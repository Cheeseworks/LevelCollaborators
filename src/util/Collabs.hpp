#pragma once

#include <API.h>

#include <matjson.hpp>

namespace cw::lvlcollab {};

template <>
struct matjson::Serialize<cw::lvlcollab::CollaboratorIcon> final {
    static geode::Result<cw::lvlcollab::CollaboratorIcon> fromJson(matjson::Value const& value);
    static matjson::Value toJson(cw::lvlcollab::CollaboratorIcon const& value);
};

template <>
struct matjson::Serialize<cw::lvlcollab::Collaborator> final {
    static geode::Result<cw::lvlcollab::Collaborator> fromJson(matjson::Value const& value);
    static matjson::Value toJson(cw::lvlcollab::Collaborator const& value);
};

template <>
struct matjson::Serialize<cw::lvlcollab::Collaboration> final {
    static geode::Result<cw::lvlcollab::Collaboration> fromJson(matjson::Value const& value);
    static matjson::Value toJson(cw::lvlcollab::Collaboration const& value);
};