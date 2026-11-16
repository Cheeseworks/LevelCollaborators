#include "../CollabPopup.hpp"

#include <Util.h>

#include <API.h>

#include <rhatt/prelude.h>

#include <cue/ListNode.hpp>

#include <Geode/Geode.hpp>

using namespace geode::prelude;
using namespace cw::lvlcollab;

bool CollabPopupCell::init(Collaborator const& collaber, bool owner, float width) {
    if (!CCNode::init()) return false;

    setAnchorPoint(cw::anchor::center);
    setContentSize({ width, 45.0f });

    auto icon = collaber.getIcon().createIcon();
    icon->setID("player-icon");

    addChildAtPosition(icon, Anchor::Left, { 25.0f, 0.0f });

    auto name = Button::createWithLabel(
        collaber.getName(),
        cw::ui::font::big,
        [id = collaber.getAccountID()](auto) {
            ProfilePage::create(id, false)->show();
        }
    );
    name->setID("view-profile-btn");
    name->setScale(std::min(0.625f, name->getScaledContentWidth() / (width - 125.0f)));
    name->setScaleMultiplier(1.125f);

    addChildAtPosition(name, Anchor::Left, { 50.0f + (name->getScaledContentWidth() * 0.5f), 7.5f });

    auto role = Label::create(fmt::format("{}{}", owner ? "Publisher | " : "", getCollabTypeName(collaber.getType())), cw::ui::font::gold);
    role->setID("type-label");
    role->setAnchorPoint({ 0.0f, 0.5f });
    role->setScale(0.475f);

    addChildAtPosition(role, Anchor::Left, { 50.0f, -8.75f });

    auto moreBtn = Button::createWithNode(
        ButtonSprite::create("More", cw::ui::font::big, owner ? "GJ_button_02.png" : "GJ_button_01.png"),
        [id = collaber.getUserID()](auto) {
            pushSceneWithLayer(LevelBrowserLayer::create(GJSearchObject::create(SearchType::UsersLevels, cw::fmtNum(id))));
        }
    );
    moreBtn->setID("view-levels-btn");
    moreBtn->setScale(0.625f);

    addChildAtPosition(moreBtn, Anchor::Right, { -(moreBtn->getScaledContentWidth() * 0.675f), 0.0f });

    if (owner) {
        auto color = CCLayerColor::create({ 0, 225, 225, 65 }, getContentWidth(), getContentHeight());
        color->setAnchorPoint(cw::anchor::center);
        color->ignoreAnchorPointForPosition(false);
        color->setZOrder(-9);

        addChildAtPosition(color, Anchor::Center);

        name->setColor({ 255, 255, 165 });
    };

    return true;
}

CollabPopupCell* CollabPopupCell::create(Collaborator const& collaber, bool owner, float width) {
    auto ret = new CollabPopupCell();
    if (ret->init(collaber, owner, width)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
}

bool CollabPopup::init(GJGameLevel* level, Collaboration const& collab) {
    if (!Popup::init({ 350.0f, 275.0f })) return false;

    setID("collaboration"_spr);
    setTitle("Collaborators");

    addSideArt(m_mainLayer, SideArt::All, SideArtStyle::PopupBlue);

    auto forLabel = Label::createRich(fmt::format("for <cc>{}</c>", collab.getLevelName()), "bigFont.fnt");
    forLabel->setID("for-level-label");
    forLabel->setScale(m_title->getScale() * 0.625f);
    forLabel->setZOrder(9);

    m_mainLayer->addChildAtPosition(forLabel, Anchor::Top, { 0.0f, -37.5f });

    auto list = cue::ListNode::createComments({ m_mainLayer->getScaledContentWidth() - 45.0f, m_mainLayer->getScaledContentHeight() - 85.0f });
    list->setID("collaber-list");

    list->addCell(CollabPopupCell::create(collab.getOwner(), true, list->getScaledContentWidth()));
    for (auto const& c : collab.getCollaborators()) list->addCell(CollabPopupCell::create(c, false, list->getScaledContentWidth()));

    m_mainLayer->addChildAtPosition(list, Anchor::Center, { 0.0f, -10.0f });

    cw::ui::addHelpButton(m_mainLayer, fmt::format("This is the <cg>collaborator list</c> for the level <cc>{}</c>. Feel free to <cy>check out everyone's creations</c>!", level->m_levelName));

    return true;
}

CollabPopup* CollabPopup::create(GJGameLevel* level, Collaboration const& collab) {
    auto ret = new CollabPopup();
    if (ret->init(level, collab)) {
        ret->autorelease();
        return ret;
    };

    delete ret;
    return nullptr;
}