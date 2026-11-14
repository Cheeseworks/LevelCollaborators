#pragma once

#include <API.h>

#include <matjson.hpp>

namespace cw::levelcollab {};

template <>
struct matjson::Serialize<cw::levelcollab::CollaboratorIcon> final {
    static geode::Result<cw::levelcollab::CollaboratorIcon> fromJson(matjson::Value const& value);
    static matjson::Value toJson(cw::levelcollab::CollaboratorIcon const& value);
};

template <>
struct matjson::Serialize<cw::levelcollab::Collaborator> final {
    static geode::Result<cw::levelcollab::Collaborator> fromJson(matjson::Value const& value);
    static matjson::Value toJson(cw::levelcollab::Collaborator const& value);
};

template <>
struct matjson::Serialize<cw::levelcollab::Collaboration> final {
    static geode::Result<cw::levelcollab::Collaboration> fromJson(matjson::Value const& value);
    static matjson::Value toJson(cw::levelcollab::Collaboration const& value);
};