#pragma once

#include <API.h>

#include <Geode/Geode.hpp>

namespace cw::lvlcollab {
    class CollabCell final : public cocos2d::CCNode {
    protected:
        bool init(Collaboration const& collaber, bool invite);

    public:
        static CollabCell* create(Collaboration const& collaber, bool invite = false);
    };
};