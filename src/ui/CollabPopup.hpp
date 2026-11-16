#pragma once

#include <API.h>

#include <Geode/Geode.hpp>

namespace cw::lvlcollab {
    class CollabPopupCell final : public cocos2d::CCNode {
    protected:
        bool init(Collaborator const& collaborator, bool owner, float width);

    public:
        static CollabPopupCell* create(Collaborator const& collaborator, bool owner, float width);
    };

    class CollabPopup final : public geode::Popup {
    protected:
        bool init(GJGameLevel* level, Collaboration const& collab);

    public:
        static CollabPopup* create(GJGameLevel* level, Collaboration const& collab);
    };
};