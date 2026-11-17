#include <API.h>

#include <ranges>

#include <Util.h>

#include <asp/iter.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace cw::lvlcollab;

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
    return CollaboratorIcon{ icon, type, color1, color2, glow, useGlow };
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
    auto gm = GameManager::sharedState();

    auto player = SimplePlayer::create(getIcon());
    player->setColor(gm->colorForIdx(getColor1()));
    player->setSecondColor(gm->colorForIdx(getColor2()));

    if (useGlow()) player->setGlowOutline(gm->colorForIdx(getGlow()));
    if (!useGlow()) player->disableGlowOutline();

    return player;
}

Collaborator::Collaborator(
    int accountID,
    int userID,
    std::string name,
    CollaboratorIcon icon,
    CollaboratorType type
) :
    m_accountID(accountID),
    m_userID(userID),
    m_name(std::move(name)),
    m_icon(icon),
    m_type(type) {}

Collaborator Collaborator::create(int accountID, int userID, std::string name, CollaboratorIcon icon, CollaboratorType type) {
    return Collaborator{ accountID, userID, std::move(name), std::move(icon), type };
}

int Collaborator::getAccountID() const noexcept {
    return m_accountID;
}

int Collaborator::getUserID() const noexcept {
    return m_userID;
}

ZStringView Collaborator::getName() const noexcept {
    return m_name;
}

CollaboratorIcon const& Collaborator::getIcon() const noexcept {
    return m_icon;
}

CollaboratorType Collaborator::getType() const noexcept {
    return m_type;
}

Collaborator::ServerResult Collaborator::reloadInfo() {
    co_return Err("");
}

Collaboration::Collaboration(
    uint64_t id,
    int levelID,
    std::string levelName,
    Collaborator owner,
    std::vector<Collaborator> collaborators
) :
    m_id(id),
    m_levelID(levelID),
    m_levelName(std::move(levelName)),
    m_owner(std::move(owner)) {
    for (auto& c : collaborators) m_collaborators[c.getAccountID()] = std::move(c);
}

Collaboration Collaboration::create(uint64_t id, int levelID, std::string levelName, Collaborator owner, std::vector<Collaborator> collaborators) {
    auto cm = CollaborationManager::get();

#ifdef CW_TESTING
    log::trace("Checking for pre-existing collaboration for level {}", levelID);
#endif

    auto cRes = cm->getCollab(levelID);
    if (cRes.isOk()) return *std::move(cRes).unwrap();

#ifdef CW_TESTING
    log::debug("Creating new collaboration for level {} with {} creators", levelID, collaborators.size());
#endif

    Collaboration c{ id, levelID, std::move(levelName), std::move(owner), std::move(collaborators) };
    cm->registerCollab(c);

    return c;
}

uint64_t Collaboration::getID() const noexcept {
    return m_id;
}

int Collaboration::getLevelID() const noexcept {
    return m_levelID;
}

ZStringView Collaboration::getLevelName() const noexcept {
    return m_levelName;
}

Collaborator const& Collaboration::getOwner() const noexcept {
    return m_owner;
}

std::map<int, Collaborator> const& Collaboration::getCollaborators() const noexcept {
    return m_collaborators;
}

Result<Collaborator const*> Collaboration::getCollaborator(int accountID) const {
    if (m_owner.getAccountID() == accountID) return Ok(&m_owner);
    if (auto const it = m_collaborators.find(accountID); it != m_collaborators.end()) return Ok(&it->second);

    return Err("No collaborator with account ID {} found", accountID);
}

std::vector<Collaborator const*> Collaboration::getAllCollaborators(bool includeOwner) const {
    std::vector<Collaborator const*> out;
    out.reserve(m_collaborators.size() + (includeOwner ? 1 : 0));

    if (includeOwner) out.push_back(&m_owner);
    for (auto const& c : m_collaborators | std::views::values) out.push_back(&c);

    std::ranges::sort(out, std::ranges::greater{}, &Collaborator::getType);

    return out;
}

std::vector<Collaborator> Collaboration::copyAllCollaborators(bool includeOwner) const {
    auto const collabers = getAllCollaborators(includeOwner);

    std::vector<Collaborator> out;
    out.reserve(collabers.size());

    for (auto const& c : collabers) out.push_back(*c);

    return out;
}

GJGameLevel* Collaboration::getGJLevel() const {
    return GameLevelManager::sharedState()->getSavedLevel(m_levelID);
}

std::string Collaboration::getFormattedString(bool prefix) const {
    auto const& owner = getOwner();
    auto const collabers = getAllCollaborators(false);

    if (collabers.size() >= 2) {
        return fmt::format("{}{} & {} more", prefix ? "By " : "", owner.getName(), collabers.size());
    } else if (collabers.size() >= 1) {
        auto other = collabers[0]->getName();
        if (other.empty()) return fmt::format("{}{} & more", prefix ? "By " : "", owner.getName());

        return fmt::format("{}{} & {}", prefix ? "By " : "", owner.getName(), other);
    } else {
        return fmt::format("{}{}", prefix ? "By " : "", owner.getName());
    };
}

struct CollaborationManager::Impl final {
    std::unordered_map<int, Collaboration> collaborations;
};

CollaborationManager::CollaborationManager() : m_impl(std::make_unique<Impl>()) {}
CollaborationManager::~CollaborationManager() = default;

void CollaborationManager::registerCollab(Collaboration collab) {
    m_impl->collaborations[collab.getLevelID()] = std::move(collab);
}

Result<Collaboration const*> CollaborationManager::getCollab(int levelID) const noexcept {
    if (auto const it = m_impl->collaborations.find(levelID); it != m_impl->collaborations.end()) return Ok(&it->second);
    return Err("Collaboration not found");
}

std::vector<Collaboration> CollaborationManager::getCollabs() const noexcept {
    std::vector<Collaboration> out;
    out.reserve(m_impl->collaborations.size());

    for (auto const& collab : m_impl->collaborations | std::views::values) out.push_back(collab);
    return out;
}

Result<Collaboration const*> CollaborationManager::getCollabForLevel(int levelID) const noexcept {
    return getCollab(levelID);
}

CollaborationManager* CollaborationManager::get() noexcept {
    static CollaborationManager inst;
    return &inst;
}

void cw::lvlcollab::requestCollabForLevel(int levelID, CopyableFunction<void(Collaboration)>&& callback) {
}

ZStringView cw::lvlcollab::getCollabTypeName(CollaboratorType type) noexcept {
    switch (type) {
        default: [[fallthrough]];

        case CollaboratorType::SpecialThanks: return "Special Thanks";
        case CollaboratorType::Playtesting: return "Playtesting";
        case CollaboratorType::Optimization: return "Optimization";
        case CollaboratorType::Effects: return "Effects";
        case CollaboratorType::Merging: return "Merging";
        case CollaboratorType::Gameplay: return "Gameplay";
        case CollaboratorType::Decoration: return "Decoration";
        case CollaboratorType::Art: return "Art";
        case CollaboratorType::Music: return "Music";
        case CollaboratorType::Verifier: return "Verifier";
    };
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