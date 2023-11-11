//
// Created by zeroc00l on 11/1/23.
//

#include <memory>
#include <string>
#include <vector>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <nlohmann/json.hpp>
#include "position.hpp"
#include "location.hpp"
#include "id.hpp"

#ifndef DGTKPROJECT_CREATURE_HPP
#define DGTKPROJECT_CREATURE_HPP

enum class CreatureType {
    Human
};

const std::vector<std::string> CreatureTypeNames = {
        {"human"}
};

enum class CreaturePropertyID {
    Name = 0,
    CreatureType,
    Location,
    TextureID,
    TexturePath,
    TexturePosition,
    TotalNumCreaturePropertyIDs
};

const std::vector<std::string> CreaturePropertyNames = {
        {"name"},
        {"creature_type"},
        {"map_location"},
        {"texture_id"},
        {"texture_path"},
        {"texture_position"}
};

struct CreatureProperties {
    CreatureType type;
    std::string name;
    MapLocation location;
    UniqueID textureID;
    std::string texturePath;
    Position texturePosition;
};

CreatureProperties ReadCreaturePropertiesFromJSON(const nlohmann::json& creatureJSON);
nlohmann::json WriteCreaturePropertiesToJSON(const CreatureProperties& creatureProperties);

struct Creature {
    CreatureProperties properties;
    Position position;
    sf::Texture* texture;
    std::unique_ptr<sf::Sprite> sprite;
};

#endif //DGTKPROJECT_CREATURE_HPP
