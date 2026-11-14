#pragma once

#include <API.h>

#include <Geode/Geode.hpp>

namespace cw::levelcollab {
    class CollabPopup final : public geode::Popup {
    protected:
        bool init(GJGameLevel* level, Collaboration const& collab);

    public:
        static CollabPopup* create(GJGameLevel* level, Collaboration const& collab);
    };
};