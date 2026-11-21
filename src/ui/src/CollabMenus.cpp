module;

#include <cw.h>

#include <API.h>

#include <rhatt/prelude.h>

#include <cue/PlayerIcon.hpp>

#include <Geode/Geode.hpp>

module cw.lvlcollab.ui.CollabMenus;

import cw.lvlcollab;

using namespace geode::prelude;
using namespace cw::lvlcollab;

bool CollabCell::init(Collaboration const& collab, float width, bool invite) {
    if (!CCNode::init()) return false;

    setAnchorPoint(cw::anchor::center);
    ignoreAnchorPointForPosition(false);
    setContentSize({ width, 40.0f });

    return true;
}

CollabCell* CollabCell::create(Collaboration const& collab, float width, bool invite) {
    auto ret = new CollabCell();
    if (ret->init(collab, width, invite)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
}

Collaboration::ServerResult CollabManagerPopup::loadCollab() {
    // pre server code

    auto arcwf = Collaborator::create(7689052, 53843371, "ArcticWoof", CollaboratorIcon::create(290, IconType::Cube, 83, 12, 3, true), CollaboratorType::Decoration);

    auto cwrks = Collaborator::create(6408873, 25777059, "Cheeseworks", CollaboratorIcon::create(28, IconType::Cube, 94, 98, 12, true), CollaboratorType::Gameplay);
    auto eri = Collaborator::create(1941705, 10709102, "Eugene", CollaboratorIcon::create(370, IconType::Cube, 21, 3, 3, true), CollaboratorType::SpecialThanks);

    co_return Ok(Collaboration::create(1, 104663075, "Seven Run", std::move(arcwf), { std::move(cwrks), std::move(eri) }));
}

bool CollabManagerPopup::init(GJGameLevel* level) {
    if (!Popup::init({ 430.0f, 275.0f }, "GE_square01.png"_gode)) return false;

    setID("manager"_spr);
    setTitle("Collaboration Manager");
    setCloseButtonSpr(CircleButtonSprite::createWithSpriteFrameName("close.png"_gode, 0.875f, CircleBaseColor::DarkPurple), 0.825f);

    auto forLabel = Label::createRich(fmt::format("for <cc>{}</c>", level->m_levelName), "bigFont.fnt");
    forLabel->setID("for-level-label");
    forLabel->setScale(m_title->getScale() * 0.625f);
    forLabel->setZOrder(1);

    m_mainLayer->addChildAtPosition(forLabel, Anchor::Top, { 0.0f, -37.5f });

    m_loading = LoadingSpinner::create(75.0f);
    m_loading->setZOrder(9);

    m_mainLayer->addChildAtPosition(m_loading, Anchor::Center);

    m_loadTask.spawn(
        loadCollab(),
        [this](Collaboration::Result) {
            m_loading->setVisible(false);
        }
    );

    cw::addHelpButton(m_mainLayer, fmt::format("This is the <cg>collaboration manager pop-up</c> for the level <cc>{}</c>. Manage the state of the collaboration for this level as you wish.", level->m_levelName));

    return true;
}

CollabManagerPopup* CollabManagerPopup::create(GJGameLevel* level) {
    auto ret = new CollabManagerPopup();
    if (ret->init(level)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
}

bool CollabLevelCell::init(GJGameLevel* level, float width) {
    if (!CCNode::init()) return false;

    setAnchorPoint(cw::anchor::center);
    ignoreAnchorPointForPosition(false);
    setContentSize({ width, 42.5f });

    cue::attachBackground(
        this,
        {
            .opacity = 255,
            .sidePadding = 0.0f,
            .verticalPadding = 0.0f,
            .texture = "GE_square01.png"_gode,
            .id = "",
        }
    );

    auto diff = GJDifficultySprite::create(cw::lvl::getDiffSpriteNum(level), GJDifficultyName::Short);
    diff->setID("difficulty-icon");
    diff->setScale(0.675f);
    diff->updateFeatureStateFromLevel(level);

    addChildAtPosition(diff, Anchor::Left, { 20.0f, 0.0f });

    auto name = Label::create(level->m_levelName, cw::font::big);
    name->setID("level-name-label");
    name->setAnchorPoint({ 0.0f, 0.5f });
    name->setLimitLabelWidth(getContentWidth() - 87.5f, 0.625f);

    addChildAtPosition(name, Anchor::Left, { 40.0f, 0.0f });

    auto lvlBtn = Button::createWithNode(
        ButtonSprite::create("View", cw::font::gold, "GE_button_05.png"_gode, 0.875f),
        [level](auto) {
            CollabManagerPopup::create(level)->show();
        }
    );
    lvlBtn->setID("view-level-btn");
    lvlBtn->setScale(0.825f);
    lvlBtn->setScaleMultiplier(1.125f);

    addChildAtPosition(lvlBtn, Anchor::Right, { -(lvlBtn->getScaledContentWidth() * 0.675f), 0.0f });

    return true;
}

CollabLevelCell* CollabLevelCell::create(GJGameLevel* level, float width) {
    auto ret = new CollabLevelCell();
    if (ret->init(level, width)) {
        ret->autorelease();
        return ret;
    }

    delete ret;
    return nullptr;
}

bool CollabInviteeCell::init(GJUserScore* user, std::optional<Collaboration> const& collab, float width) {
    if (!CCNode::init()) return false;

    setAnchorPoint(cw::anchor::center);
    ignoreAnchorPointForPosition(false);
    setContentSize({ width, 40.0f });

    auto icon = ui::createPlayerIconFromScore(user);
    icon->setID("player-icon");

    addChildAtPosition(icon, Anchor::Left, { -20.0f, 0.0f });

    auto name = cw::createProfileBtn(user);
    name->setScale(std::min(0.625f, name->getScaledContentWidth() / (getContentWidth() - 125.0f)));

    addChildAtPosition(name, Anchor::Left, { 50.0f + (name->getScaledContentWidth() / 2.0f), 0.0f });

    if (collab.has_value()) {
    } else {
    };

    return true;
}

CollabInviteeCell* CollabInviteeCell::create(GJUserScore* user, std::optional<Collaboration> const& collab, float width) {
    auto ret = new CollabInviteeCell();
    if (ret->init(user, collab, width)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
}