#include "../CollabDashboard.hpp"

#include <Util.h>

#include <API.h>

#include <rhatt/prelude.h>

#include <cue/RepeatingBackground.hpp>

#include <Geode/Geode.hpp>

#include <Geode/ui/Style.hpp>

using namespace geode::prelude;
using namespace cw::levelcollab;

bool CollabDashboardMenu::init(CCSize const& size) {
    if (!CCNode::init()) return false;

    setContentSize(size);
    setAnchorPoint({ 0.5f, 0.5f });
    ignoreAnchorPointForPosition(false);
    setVisible(false);

    return true;
}

bool CollabDashboard::init() {
    if (!CCLayer::init()) return false;

    setKeypadEnabled(true);
    setKeyboardEnabled(true);

    addBackButton(this, BackButtonStyle::Blue);

    auto const winSize = CCDirector::sharedDirector()->getWinSize();

    auto bg = cue::RepeatingBackground::create("game_bg_12_001.png", 0.875f, cue::RepeatMode::X, winSize);
    bg->setColor({ 187, 254, 215 });
    bg->setSpeed(1.125f);

    addChild(bg, -9);

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
    btnContainer->setAnchorPoint({ 1, 0 });
    btnContainer->setLayout(btnContainerLayout);

    addChildAtPosition(btnContainer, Anchor::BottomRight, { -12.5f, 12.5f }, false);

    auto btns = std::to_array<cw::ui::LinkButton>(
        {
            {
                "kofi-btn",
                "gift.png"_gode,
                [](auto) {
                    openSupportPopup(Mod::get());
                },
            },
            {
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
        }
    );

    for (auto& b : btns) {
        auto btn = Button::createWithSpriteFrameName(
            b.sprite,
            std::move(b.callback)
        );
        btn->setID(std::move(b.id));

        btnContainer->addChild(btn);
    };

    btnContainer->updateLayout();

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