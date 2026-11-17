#pragma once

#include <cue/PlayerIcon.hpp>

#include <Geode/Geode.hpp>

namespace cw::lvlcollab {
    namespace ui {
        inline auto createPlayerIconFromScore(GJUserScore* user) {
            return cue::PlayerIcon::create(user->m_iconType, user->m_iconID, user->m_color1, user->m_color2, user->m_color3);
        }
    };
};