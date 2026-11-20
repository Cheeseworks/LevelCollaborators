#include "../Collabs.hpp"

#include <cw.h>

#include <rhatt/prelude.h>

#include <Geode/Geode.hpp>

import cw.lvlcollab;

using namespace geode::prelude;
using namespace cw::lvlcollab;

Result<CollaboratorIcon> matjson::Serialize<CollaboratorIcon>::fromJson(matjson::Value const& value) {
    GEODE_UNWRAP_INTO(auto icon, value["icon"].asUInt());
    GEODE_UNWRAP_INTO(auto _type, value["icon_type"].asInt());
    GEODE_UNWRAP_INTO(auto color1, value["color1"].asInt());
    GEODE_UNWRAP_INTO(auto color2, value["color2"].asInt());
    GEODE_UNWRAP_INTO(auto glow, value["color_glow"].asInt());
    GEODE_UNWRAP_INTO(auto useGlow, value["use_glow"].asBool());

    auto type = (IconType)_type;

    return Ok(CollaboratorIcon::create(icon, type, color1, color2, glow, useGlow));
}

matjson::Value matjson::Serialize<CollaboratorIcon>::toJson(CollaboratorIcon const& value) {
    Value out;
    out["icon"] = value.getIcon();
    out["icon_type"] = (int)value.getIconType();
    out["color1"] = value.getColor1();
    out["color2"] = value.getColor2();
    out["color_glow"] = value.getGlow();
    out["use_glow"] = value.useGlow();

    return out;
}

Result<Collaborator> matjson::Serialize<Collaborator>::fromJson(matjson::Value const& value) {
    GEODE_UNWRAP_INTO(auto accountID, value["account_id"].asInt());
    GEODE_UNWRAP_INTO(auto userID, value["user_id"].asInt());
    GEODE_UNWRAP_INTO(auto name, value["name"].asString());
    GEODE_UNWRAP_INTO(auto icon, value["icon"].as<CollaboratorIcon>());
    GEODE_UNWRAP_INTO(auto _type, value["type"].asUInt());

    auto type = (CollaboratorType)_type;

    return Ok(Collaborator::create(accountID, userID, std::move(name), icon, type));
}

matjson::Value matjson::Serialize<Collaborator>::toJson(Collaborator const& value) {
    Value out;
    out["account_id"] = value.getAccountID();
    out["user_id"] = value.getUserID();
    out["name"] = value.getName();
    out["icon"] = value.getIcon();
    out["type"] = (uint8_t)value.getType();

    return out;
}

Result<Collaboration> matjson::Serialize<Collaboration>::fromJson(matjson::Value const& value) {
    GEODE_UNWRAP_INTO(auto id, value["id"].asUInt());
    GEODE_UNWRAP_INTO(auto levelID, value["level_id"].asInt());
    GEODE_UNWRAP_INTO(auto levelName, value["level_name"].asString());
    GEODE_UNWRAP_INTO(auto owner, value["owner"].as<Collaborator>());
    GEODE_UNWRAP_INTO(auto _collaborators, value["collaborators"].asArray());

    std::vector<Collaborator> collaborators;
    for (auto& val : _collaborators) {
        GEODE_UNWRAP_INTO(auto c, val.as<Collaborator>());
        collaborators.push_back(std::move(c));
    };

    return Ok(Collaboration::create(id, levelID, std::move(levelName), std::move(owner), std::move(collaborators)));
}

matjson::Value matjson::Serialize<Collaboration>::toJson(Collaboration const& value) {
    Value out;
    out["id"] = value.getID();
    out["level_id"] = value.getLevelID();
    out["level_name"] = value.getLevelName();
    out["owner"] = value.getOwner();
    out["collaborators"] = value.copyAllCollaborators();

    return out;
}