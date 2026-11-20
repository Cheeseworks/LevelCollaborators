module;

#include <API.h>

#include <Geode/Geode.hpp>

export module cw.lvlcollab.ui.CollabMenus;

export namespace cw::lvlcollab {
    class CollabCell final : public cocos2d::CCNode {
    protected:
        bool init(Collaboration const& collab, float width, bool invite);

    public:
        static CollabCell* create(Collaboration const& collab, float width, bool invite = false);
    };

    class CollabManagerPopup final : public geode::Popup {
    protected:
        bool init(GJGameLevel* level);

    public:
        static CollabManagerPopup* create(GJGameLevel* level);
    };

    class CollabLevelCell final : public cocos2d::CCNode {
    protected:
        bool init(GJGameLevel* level, float width);

    public:
        static CollabLevelCell* create(GJGameLevel* level, float width);
    };

    class CollabInviteeCell final : public cocos2d::CCNode {
    protected:
        bool init(GJUserScore* user, std::optional<Collaboration> const& collab, float width);

    public:
        static CollabInviteeCell* create(GJUserScore* user, std::optional<Collaboration> const& collab, float width);
    };
};