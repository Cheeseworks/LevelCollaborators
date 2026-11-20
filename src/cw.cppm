module;

#include <cw.h>

export module cw.lvlcollab;

export import cw.lvlcollab.ui;
export import cw.lvlcollab.util;

export namespace cw::lvlcollab {
    namespace manager {  // very wip
        void requestCollabUpsert(int levelID, Collaboration collab, geode::CopyableFunction<void(bool success, std::string const& message)>&& callback = nullptr);
    };

    namespace fetch {
        void getUserByUsername(std::string_view username, geode::CopyableFunction<void(geode::Result<GJUserScore*>)>&& callback);
    };
};