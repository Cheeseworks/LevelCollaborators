#pragma once

#include <util/Macros.h>

#include <API.h>

#include <ui/Include.h>
#include <util/Include.h>

#include <cue/Util.hpp>

#include <Geode/Geode.hpp>

namespace cw::lvlcollab {
    namespace manager {  // very wip
        void requestCollabUpsert(int levelID, Collaboration collab, geode::CopyableFunction<void(bool success, std::string const& message)>&& callback = nullptr);
    };

    namespace fetch {
        void getUserByUsername(std::string_view username, geode::CopyableFunction<void(geode::Result<GJUserScore*>)>&& callback);
    };
};