#include <API.h>

#include <Util.h>

#include <asp/iter.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace cw::levelcollab;

CollaboratorIcon::CollaboratorIcon(
    int icon,
    IconType type,
    int color1,
    int color2,
    int glow,
    bool useGlow
) :
    m_icon(icon),
    m_type(type),
    m_color1(color1),
    m_color2(color2),
    m_glow(glow),
    m_useGlow(useGlow) {}

CollaboratorIcon CollaboratorIcon::create(int icon, IconType type, int color1, int color2, int glow, bool useGlow) {
    return CollaboratorIcon(icon, type, color1, color2, glow, useGlow);
}

int CollaboratorIcon::getIcon() const noexcept {
    return m_icon;
}

IconType CollaboratorIcon::getIconType() const noexcept {
    return m_type;
}

int CollaboratorIcon::getColor1() const noexcept {
    return m_color1;
}

int CollaboratorIcon::getColor2() const noexcept {
    return m_color2;
}

int CollaboratorIcon::getGlow() const noexcept {
    return m_glow;
}

bool CollaboratorIcon::useGlow() const noexcept {
    return m_useGlow;
}

SimplePlayer* CollaboratorIcon::createIcon() const {
    // me when gamemanager doesnt exist owo (/j (you're gonna see this a lot just ignore it))
    if (auto gm = GameManager::sharedState()) {
        auto player = SimplePlayer::create(getIcon());
        player->setColor(gm->colorForIdx(getColor1()));
        player->setSecondColor(gm->colorForIdx(getColor2()));

        if (useGlow()) player->setGlowOutline(gm->colorForIdx(getGlow()));
        if (!useGlow()) player->disableGlowOutline();

        return player;
    };

    return nullptr;
}

Collaborator::Collaborator(
    std::string name,
    int accountID,
    CollaboratorIcon icon,
    CollaboratorType type
) :
    m_name(std::move(name)),
    m_accountID(accountID),
    m_icon(icon),
    m_type(type) {}

Collaborator Collaborator::create(std::string name, int accountID, CollaboratorIcon icon, CollaboratorType type) {
    return Collaborator(std::move(name), accountID, std::move(icon), type);
}

ZStringView Collaborator::getName() const noexcept {
    return m_name;
}

int Collaborator::getAccountID() const noexcept {
    return m_accountID;
}

CollaboratorIcon const& Collaborator::getIcon() const noexcept {
    return m_icon;
}

CollaboratorType Collaborator::getType() const noexcept {
    return m_type;
}

void Collaborator::reloadInfo(geode::CopyableFunction<void(geode::Result<Collaborator>)>&& callback) {
    getCollaboratorInfo(getAccountID(), [cb = std::move(callback), this](geode::Result<GJUserScore*> res) {
        if (res.isErr()) {
            log::error("Failed to reload collaborator info: {}", res.unwrapErr());
            return cb(Err(res.unwrapErr()));
        };

        auto userInfo = res.unwrapOrDefault();

        m_name = userInfo->m_userName;
        m_accountID = userInfo->m_accountID;
        m_icon = CollaboratorIcon::create(userInfo->m_iconID, userInfo->m_iconType, userInfo->m_color1, userInfo->m_color2, userInfo->m_color3, userInfo->m_glowEnabled);

        cb(Ok(*this));
    });
}

Collaboration::Collaboration(
    int levelID,
    Collaborator owner,
    std::vector<Collaborator> collaborators
) :
    m_levelID(levelID),
    m_owner(std::move(owner)),
    m_collaborators(std::move(collaborators)) {}

Collaboration Collaboration::create(int levelID, Collaborator owner, std::vector<Collaborator> collaborators) {
    auto cm = CollaborationManager::get();

#ifdef CW_TESTING
    log::trace("Checking for pre-existing collaboration for level {}", levelID);
#endif

    auto cRes = cm->getCollab(levelID);
    if (cRes.isOk()) return std::move(cRes).unwrap();

#ifdef CW_TESTING
    log::debug("Creating new collaboration for level {} with {} creators", levelID, collaborators.size());
#endif

    auto c = Collaboration{ levelID, std::move(owner), std::move(collaborators) };
    cm->registerCollab(c);

    return c;
}

int Collaboration::getLevelID() const noexcept {
    return m_levelID;
}

Collaborator const& Collaboration::getOwner() const noexcept {
    return m_owner;
}

std::span<const Collaborator> Collaboration::getCollaborators() const noexcept {
    return m_collaborators;
}

GJGameLevel* Collaboration::getLevel() const {
    if (auto glm = GameLevelManager::sharedState()) return glm->getSavedLevel(m_levelID);
    return nullptr;
}

std::string Collaboration::getFormattedString(bool prefix) const {
    std::string out;

    auto const& owner = getOwner();

    if (getCollaborators().size() > 2) {
        out = fmt::format("{}{} & {} more", prefix ? "By " : "", owner.getName(), getCollaborators().size() - 1);
    } else if (getCollaborators().size() >= 1) {
        std::string other = getCollaborators()[0].getName();

        if (other.empty()) return fmt::format("{}{} & more", prefix ? "By " : "", owner.getName());

        out = fmt::format("{}{} & {}", prefix ? "By " : "", owner.getName(), other);
    } else {
        return fmt::format("{}{}", prefix ? "By " : "", owner.getName());
    };

    return out;
}

struct CollaborationManager::Impl final {
    std::unordered_map<int, Collaboration> collaborations;
};

CollaborationManager::CollaborationManager() : m_impl(std::make_unique<Impl>()) {}
CollaborationManager::~CollaborationManager() = default;

void CollaborationManager::registerCollab(Collaboration collab) {
    m_impl->collaborations[collab.getLevelID()] = std::move(collab);
}

Result<Collaboration> CollaborationManager::getCollab(int levelID) const noexcept {
    if (auto it = m_impl->collaborations.find(levelID); it != m_impl->collaborations.end()) return Ok(it->second);
    return Err("Collaboration not found");
}

std::vector<Collaboration> CollaborationManager::getCollabs() const noexcept {
    std::vector<Collaboration> out;
    out.reserve(m_impl->collaborations.size());

    for (auto const& collab : m_impl->collaborations) out.push_back(collab.second);
    return out;
}

geode::Result<Collaboration> CollaborationManager::getCollabForLevel(int levelID) const noexcept {
    return getCollab(levelID);
}

CollaborationManager* CollaborationManager::get() noexcept {
    static CollaborationManager inst;
    return &inst;
}

void cw::levelcollab::requestCollabForLevel(int levelID, CopyableFunction<void(Collaboration)>&& callback) {
}

void cw::levelcollab::getCollaboratorInfo(int accountID, CopyableFunction<void(Result<GJUserScore*>)>&& callback) {
    auto req = web::WebRequest()
                   .bodyString(fmt::format("secret=Wmfd2893gb7&targetAccountID={}", utils::numToString(accountID)))
                   .userAgent("");

    async::spawn(
        req.post("https://www.boomlings.com/database/getGJUserInfo20.php"),
        [cb = std::move(callback)](web::WebResponse res) {  // copyable has const () operator woohoo!
            auto const resStr = res.string().unwrapOrDefault();

            if (res.error()) {
                log::error("Error getting user information: {}", resStr);
                return cb(Err("An error occurred while fetching user information"));
            };

#ifdef CW_TESTING
            log::debug("Received response: {}", resStr);
#endif

            auto dict = CCDictionary::create();
            auto splits = asp::iter::split(resStr, ":")  // owo
                              .mapCast<std::string>()
                              .collect();

            for (size_t i = 0; i + 1 < splits.size(); i += 2) dict->setObject(CCString::create(splits[i + 1]), splits[i]);
            cb(Ok(GJUserScore::create(dict)));
        }
    );
}

void fetch::getUserByUsername(std::string_view username, CopyableFunction<void(Result<GJUserScore*>)>&& callback) {
    auto req = web::WebRequest()
                   .bodyString(fmt::format("secret=Wmfd2893gb7&str={}", username))
                   .userAgent("");

    async::spawn(
        req.post("https://www.boomlings.com/database/getGJUsers20.php"),
        [cb = std::move(callback)](web::WebResponse res) {
            auto const resStr = res.string().unwrapOrDefault();

            if (res.error()) {
                log::error("Error getting user information: {}", resStr);
                return cb(Err("An error occurred while fetching user information"));
            };

#ifdef CW_TESTING
            log::debug("Received response: {}", resStr);
#endif

            auto dict = CCDictionary::create();
            auto splits = asp::iter::split(resStr, ":")
                              .mapCast<std::string>()
                              .collect();

            for (size_t i = 0; i + 1 < splits.size(); i += 2) dict->setObject(CCString::create(splits[i + 1]), splits[i]);
            cb(Ok(GJUserScore::create(dict)));
        }
    );
}