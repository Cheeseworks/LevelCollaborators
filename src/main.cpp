#include <API.h>

#include <Util.h>

#include <rhatt/prelude.h>

#include <dankmeme.argon/argon.hpp>

#include <Geode/Geode.hpp>

#include <Geode/ui/Button.hpp>

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;
using namespace cw::levelcollab;

// just for testing
static auto const arcwf = Collaborator::create("ArcticWoof", 7689052, CollaboratorIcon::create(28, IconType::Cube, 94, 98, 1, true), CollaboratorType::Decoration);
static auto const cwrks = Collaborator::create("Cheeseworks", 6408873, CollaboratorIcon::create(290, IconType::Cube, 83, 12, 3, true), CollaboratorType::Gameplay);

static auto const collaboration = Collaboration::create(104663075, arcwf, { cwrks });

namespace cw::levelcollab {
    namespace ui {
        static void addCollabButton(CCNode* menu, bool alt = true) {
            auto btn = CCMenuItemExt::createSpriteExtra(
                CircleButtonSprite::createWithSpriteFrameName(
                    "GJ_sFriendsIcon_001.png",
                    0.875f,
                    CircleBaseColor::Green,
                    alt ? CircleBaseSize::MediumAlt : CircleBaseSize::Medium
                ),
                [](auto) {
                    pushSceneWithLayer(CollabDashboard::create());
                }
            );
            btn->setID("collab-dashboard-btn"_spr);

            menu->addChild(btn);
            menu->updateLayout();
        }
    };
};

class $modify(LCMenuLayer, MenuLayer) {
    bool init() {
        if (!MenuLayer::init()) return false;

        if (auto menu = getChildByID("bottom-menu")) ui::addCollabButton(menu);

        return true;
    }
};

// class $modify(LCLevelCell, LevelCell) {
//     void loadFromLevel(GJGameLevel* level) {
//         LevelCell::loadFromLevel(level);
//     };
// };

class $modify(LCLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        if (auto menu = getChildByID("left-side-menu")) ui::addCollabButton(menu, false);

        auto cm = CollaborationManager::get();

        auto collabRes = cm->getCollabForLevel(m_level->m_levelID);
        if (collabRes.isOk()) {
            auto collab = std::move(collabRes).unwrap();

            auto creatorMenu = getChildByID("creator-info-menu");

            auto btnLabel = Label::create(collab.getFormattedString(), cw::ui::font::gold);
            btnLabel->setLimitLabelWidth(37.5f, 0.75f);

            auto btn = Button::createWithNode(
                btnLabel,
                [this, collab = std::move(collab)](auto) {
                    CollabPopup::create(m_level, collab)->show();
                }
            );
            btn->setID("collaboration-btn"_spr);
            btn->setScale(0.75f);
            btn->setPosition({ creatorMenu->getPositionX(), creatorMenu->getPositionY() + 11.25f });

            if (auto copyTag = getChildByID("copy-indicator")) {
                copyTag->setPositionX((getScaledContentWidth() / 2.0f) + (btn->getScaledContentWidth() / 2.0f) + 5.0f);
                if (auto moreTags = getChildByID("raydeeux_thesillydoggo.evenmoreleveltags/more-level-tags-menu")) moreTags->setPositionX(copyTag->getPositionX() + 17.5f);
            };

            creatorMenu->setVisible(false);
            addChild(btn);
        }

        return true;
    }
};