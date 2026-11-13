#pragma once

#include <util/Macros.h>

#include <API.h>

#include <Geode/Geode.hpp>

namespace cw::levelcollab {
    namespace manager {  // very wip
        void requestCollabUpsert(int levelID, std::weak_ptr<Collaboration> collab, geode::CopyableFunction<void(bool success, std::string const& message)>&& callback = nullptr);
    };

    namespace fetch {
        void getUserByUsername(std::string_view username, geode::CopyableFunction<void(geode::Result<GJUserScore*>)>&& callback);
    };
};