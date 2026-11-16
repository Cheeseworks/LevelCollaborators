#include "../CollabMenus.hpp"

#include <Util.h>

#include <API.h>

#include <rhatt/prelude.h>

#include <cue/PlayerIcon.hpp>

#include <Geode/Geode.hpp>

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

bool CollabLevelCell::init(GJGameLevel* level, float width) {
    if (!CCNode::init()) return false;

    setAnchorPoint(cw::anchor::center);
    ignoreAnchorPointForPosition(false);
    setContentSize({ width, 42.5f });

    cue::attachBackground(
        this,
        {
            .sidePadding = 0.f,
            .verticalPadding = 0.f,
            .texture = "white-square.png"_gode,
            .id = "",
        }
    );

    auto diff = GJDifficultySprite::create(cw::lvl::getDiffSpriteNum(level), GJDifficultyName::Short);
    diff->setID("difficulty-icon");
    diff->updateFeatureStateFromLevel(level);

    addChildAtPosition(diff, Anchor::Left, { 25.0f, 0.0f });

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

    auto name = cw::ui::createProfileBtn(user);
    name->setScale(std::min(0.625f, name->getScaledContentWidth() / (getContentWidth() - 125.0f)));

    addChildAtPosition(name, Anchor::Left, { 50.0f + (name->getScaledContentWidth() * 0.5f), 0.0f });

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