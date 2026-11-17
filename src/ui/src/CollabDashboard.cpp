#include "../CollabDashboard.hpp"

#include <Util.h>

#include <API.h>

#include <rhatt/prelude.h>

#include <cue/RepeatingBackground.hpp>

#include <Geode/Geode.hpp>

#include <Geode/ui/Style.hpp>

using namespace geode::prelude;
using namespace cw::lvlcollab;

bool CollabDashboardMenu::init(CCSize const& size) {
    if (!CCNode::init()) return false;

    setContentSize(size);
    setAnchorPoint(cw::anchor::center);
    ignoreAnchorPointForPosition(false);
    setVisible(false);

    return true;
}

bool menu::DashboardInvites::init(CCSize const& size) {
    if (!CollabDashboardMenu::init(size)) return false;

    EmojiRegistry emojis;
    emojis.insert(U":sus:", "particle_318_001.png");

    auto nothing = Label::create("No invites yet... :sus:", cw::ui::font::gold);
    nothing->setID("nothing-yet-label");
    nothing->setScale(0.625f);
    nothing->setAlignment(Label::Alignment::Center);
    nothing->setEmojiRegistry(emojis);

    addChildAtPosition(nothing, Anchor::Center);

    return true;
}

menu::DashboardInvites* menu::DashboardInvites::create(CCSize const& size) {
    auto ret = new menu::DashboardInvites();
    if (ret->init(size)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

CCArrayExt<GJGameLevel*> menu::DashboardManage::s_levels;

void menu::DashboardManage::showUI() {
    m_loading->setVisible(false);

    m_nothingLabel->setVisible(s_levels.empty());
    m_levelList->setVisible(!s_levels.empty());
}

void menu::DashboardManage::loadLevelList(bool error) {
    m_loadingPage = 0;

    for (auto const& lvl : s_levels) m_levelList->m_contentLayer->addChild(CollabLevelCell::create(lvl, m_levelList->getScaledContentWidth()));
    m_levelList->m_contentLayer->updateLayout();
    m_levelList->scrollToTop();

    showUI();
}

void menu::DashboardManage::requestNextBatch() {
    DelegateDirector::get()->searchUserLevels(25777059, m_loadingPage, [self = WeakRef(this)](Result<CCArrayExt<GJGameLevel*>> res) {
        if (auto s = self.lock()) {
            if (res.isErr()) {
                log::error("Failed to search for user: {}", res.unwrapErr());
                s->loadLevelList(true);

                return;
            };

            auto const levels = std::move(res).unwrap();
            log::info("Found {} levels!", levels.size());

            if (levels.empty()) return s->loadLevelList();

            for (auto const& lvl : levels) {
                if (!lvl->m_unlisted || !lvl->m_friendsOnly) menu::DashboardManage::s_levels.push_back(lvl);
            };

            if (levels.size() >= 10) {
                s->m_loadingPage++;
                s->requestNextBatch();

                return;
            };

            s->loadLevelList();
        };
    });
}

bool menu::DashboardManage::init(CCSize const& size) {
    if (!CollabDashboardMenu::init(size)) return false;

    m_levelList = ScrollLayer::create(size * 0.925f);
    m_levelList->setID("level-list");
    m_levelList->setVisible(false);

    m_levelList->m_contentLayer->setLayout(ScrollLayer::createDefaultListLayout(3.75f));

    addChildAtPosition(m_levelList, Anchor::Center);

    m_levelList->m_contentLayer->updateLayout();
    m_levelList->scrollToTop();

    m_loading = LoadingSpinner::create(32.5f);
    m_loading->setZOrder(1);

    addChildAtPosition(m_loading, Anchor::Center);

    m_nothingLabel = Label::create("You have no public levels available.", cw::ui::font::gold);
    m_nothingLabel->setID("nothing-yet-label");
    m_nothingLabel->setScale(0.625f);
    m_nothingLabel->setAlignment(Label::Alignment::Center);
    m_nothingLabel->setVisible(false);
    m_nothingLabel->setZOrder(9);

    addChildAtPosition(m_nothingLabel, Anchor::Center);

    if (s_levels.empty()) {
        requestNextBatch();
    } else {
        showUI();
    };

    return true;
}

menu::DashboardManage* menu::DashboardManage::create(CCSize const& size) {
    auto ret = new menu::DashboardManage();
    if (ret->init(size)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool menu::DashboardCredits::init(CCSize const& size) {
    if (!CollabDashboardMenu::init(size)) return false;

    return true;
}

menu::DashboardCredits* menu::DashboardCredits::create(CCSize const& size) {
    auto ret = new menu::DashboardCredits();
    if (ret->init(size)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

void CollabDashboard::switchMenu(CollabMenuType type) {
    for (auto const& [menu, node] : m_menus) node->setVisible(menu == type);
    for (auto const& [tab, node] : m_tabs) node->select(tab == type);
}

bool CollabDashboard::init() {
    if (!CCLayer::init()) return false;

    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    addBackButton(this, BackButtonStyle::Pink);

    auto const winSize = CCDirector::sharedDirector()->getWinSize();

    auto bg = cue::RepeatingBackground::create("game_bg_12_001.png", 0.875f, cue::RepeatMode::X, winSize);
    bg->setColor({ 75, 75, 75 });
    bg->setSpeed(1.125f);

    addChild(bg, -9);

    auto bgGrad = CCLayerGradient::create({ 0, 65, 115, 125 }, { 15, 105, 10, 125 }, { 45.0f, -45.0f });
    bgGrad->setBlendFunc({ GL_ONE, GL_ONE_MINUS_SRC_ALPHA });
    bgGrad->setOpacity(100);

    addChild(bgGrad, -8);

    auto title = Button::createWithLabel(
        "Level Collaborations",
        cw::ui::font::gold,
        [](auto) {
            openInfoPopup(Mod::get());
        }
    );
    title->setID("view-mod-info-btn");
    title->setScaleMultiplier(1.125f);

    addChildAtPosition(title, Anchor::Top, { 0.0f, -1.0f * (title->getScaledContentHeight() * 0.875f) }, false);

    auto btnContainerLayout = ColumnLayout::create()
                                  ->setGap(5.0f)
                                  ->setAutoScale(false)
                                  ->setAutoGrowAxis(1.25f)
                                  ->setAxisReverse(true);

    auto btnContainer = CCNode::create();
    btnContainer->setID("social-button-menu");
    btnContainer->setAnchorPoint({ 1.0f, 0.0f });
    btnContainer->setLayout(btnContainerLayout);

    addChildAtPosition(btnContainer, Anchor::BottomRight, { -12.5f, 12.5f }, false);

    auto btns = std::array{
        cw::ui::LinkButton{
            "kofi-btn",
            "gift.png"_gode,
            [](auto) {
                openSupportPopup(Mod::get());
            },
        },
        cw::ui::LinkButton{
            "discord-btn",
            "gj_discordIcon_001.png",
            [](auto) {
                createQuickPopup(
                    "Discord Community",
                    "Join <cd>Cheeseworks</c>'s <cb>Discord server</c>?\n"
                    "<cs>Get help, report bugs, and chat with other players!</c>",
                    "Cancel",
                    "OK",
                    [](auto, bool ok) {
                        if (ok) web::openLinkInBrowser("https://www.dsc.gg/cheeseworks");
                    }
                );
            },
        },
    };

    for (auto& b : btns) {
        auto btn = Button::createWithSpriteFrameName(
            b.sprite,
            std::move(b.callback)
        );
        btn->setID(std::move(b.id));

        btnContainer->addChild(btn);
    };

    btnContainer->updateLayout();

    static constexpr auto maxContainerWidth = 440.0f;
    static constexpr auto minSidesGap = 50.0f;

    auto containerWidth = std::min(maxContainerWidth, winSize.width - minSidesGap * 2.0f);

    auto menuContainer = NineSlice::create("black-square.png"_gode);
    menuContainer->setID("menu-container");
    menuContainer->setContentSize({ containerWidth, 230.0f });

    addChildAtPosition(menuContainer, Anchor::Center, { 0.0f, -1.25f }, false);

    auto const dbSize = menuContainer->getScaledContentSize();

    struct DashboardData final {
        CollabMenuType type;
        CollabDashboardMenu* menu;
    };

    auto dbs = std::array{
        DashboardData{
            CollabMenuType::Invites,
            menu::DashboardInvites::create(dbSize),
        },
        DashboardData{
            CollabMenuType::Manage,
            menu::DashboardManage::create(dbSize),
        },
        DashboardData{
            CollabMenuType::Credits,
            menu::DashboardCredits::create(dbSize),
        },
    };

    for (auto const& db : dbs) {
        m_menus[db.type] = db.menu;
        menuContainer->addChildAtPosition(db.menu, Anchor::Center);
    };

    auto tabContainerLayout = RowLayout::create()
                                  ->setGap(15.0f)
                                  ->setDefaultScaleLimits(0.125f, 0.825f);

    auto tabContainer = CCNode::create();
    tabContainer->setID("tab-container");
    tabContainer->setAnchorPoint(cw::anchor::center);
    tabContainer->setContentWidth(containerWidth);
    tabContainer->setLayout(tabContainerLayout);

    addChildAtPosition(tabContainer, Anchor::Bottom, { 0.0f, 22.5f }, false);

    struct TabData final {
        std::string id;
        ZStringView sprite;
        std::string label;
        CollabMenuType tab;
    };

    auto tabs = std::array{
        TabData{
            "invite-viewer-tab-btn",
            "particle_207_001.png",
            "Invites",
            CollabMenuType::Invites,
        },
        TabData{
            "collab-manager-tab-btn",
            "particle_200_001.png",
            "Manage",
            CollabMenuType::Manage,
        },
        TabData{
            "credits-btn",
            "particle_218_001.png",
            "Credits",
            CollabMenuType::Credits,
        },
    };

    for (auto& t : tabs) {
        auto spr = GeodeTabSprite::create(t.sprite, std::move(t.label), 130.0f);
        auto btn = Button::createWithNode(
            spr,
            [this, tab = t.tab](auto) {
                this->switchMenu(tab);
            }
        );
        btn->setID(std::move(t.id));
        btn->setScaleMultiplier(1.125f);

        tabContainer->addChild(btn);

        m_tabs[t.tab] = spr;
    };

    tabContainer->updateLayout();

    cw::ui::addHelpButton(this, "This is your <cg>Level Collab dashboard</c>, you can <cy>manage your own level collaborations</c> or <cy>review invites from others</c>.", false, 0.875f, { -7.5f, -7.5f });

    switchMenu(CollabMenuType::Invites);

    return true;
}

void CollabDashboard::keyBackClicked() {
    CCDirector::sharedDirector()->popSceneWithTransition(0.5f, PopTransition::kPopTransitionFade);
}

CollabDashboard* CollabDashboard::create() {
    auto ret = new CollabDashboard();
    if (ret->init()) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
}