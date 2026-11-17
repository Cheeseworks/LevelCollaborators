#include <API.h>

#include <Util.h>

#include <rhatt/prelude.h>

#include <Geode/Geode.hpp>

#include <Geode/ui/Button.hpp>

#include <Geode/modify/MenuLayer.hpp>
#include <Geode/modify/LevelCell.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>

using namespace geode::prelude;
using namespace cw::lvlcollab;

$on_mod(Loaded) {  // just for testing
    auto arcwf = Collaborator::create(7689052, 53843371, "ArcticWoof", CollaboratorIcon::create(290, IconType::Cube, 83, 12, 3, true), CollaboratorType::Decoration);

    auto cwrks = Collaborator::create(6408873, 25777059, "Cheeseworks", CollaboratorIcon::create(28, IconType::Cube, 94, 98, 12, true), CollaboratorType::Gameplay);
    auto eri = Collaborator::create(1941705, 10709102, "Eugene", CollaboratorIcon::create(370, IconType::Cube, 21, 3, 3, true), CollaboratorType::SpecialThanks);

    Collaboration::create(1, 104663075, "Seven Run", std::move(arcwf), { std::move(cwrks), std::move(eri) });
}

namespace cw::lvlcollab {
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

        queueInMainThread([]() {
            DelegateDirector::get()->searchUserByID(6408873, [](Result<GJUserScore*> res) {
                if (res.isErr()) return log::error("Failed to search for user: {}", res.unwrapErr());

                auto u = std::move(res).unwrap();
                log::info("Found user {} ({} ({})) #{}", u->m_userName, u->m_accountID, u->m_userID, u->m_globalRank);
            });
        });

        return true;
    }
};

class $modify(LCLevelCell, LevelCell) {
    void loadFromLevel(GJGameLevel* level) {
        LevelCell::loadFromLevel(level);

        auto cm = CollaborationManager::get();

        auto collabRes = cm->getCollabForLevel(m_level->m_levelID);
        if (collabRes.isOk()) {
            auto collab = std::move(collabRes).unwrap();

            auto nameLabel = m_mainLayer->getChildByID("level-name");

            auto btn = Button::createWithLabel(
                collab->getFormattedString(),
                cw::ui::font::gold,
                [this, collab](auto) {
                    CollabPopup::create(m_level, *collab)->show();
                }
            );
            btn->setID("collaboration-btn"_spr);
            btn->setScale(std::min(0.65f, btn->getScaledContentWidth() / 12.5f));
            btn->setPosition({ nameLabel->getPositionX() + (btn->getScaledContentWidth() * 0.5f) + 1.25f, nameLabel->getPositionY() - 22.5f });

            m_mainLayer->addChild(btn, 1);

            m_mainMenu->getChildByID("creator-name")->setVisible(false);
        };
    }
};

class $modify(LCLevelInfoLayer, LevelInfoLayer) {
    bool init(GJGameLevel* level, bool challenge) {
        if (!LevelInfoLayer::init(level, challenge)) return false;

        if (auto menu = getChildByID("left-side-menu")) ui::addCollabButton(menu, false);

        auto cm = CollaborationManager::get();

        auto collabRes = cm->getCollabForLevel(m_level->m_levelID);
        if (collabRes.isOk()) {
            auto collab = std::move(collabRes).unwrap();

            auto creatorMenu = getChildByID("creator-info-menu");

            auto btn = Button::createWithLabel(
                collab->getFormattedString(),
                cw::ui::font::gold,
                [this, collab](auto) {
                    CollabPopup::create(m_level, *collab)->show();
                }
            );
            btn->setID("collaboration-btn"_spr);
            btn->setScale(std::min(0.75f, btn->getScaledContentWidth() / 75.0f));
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