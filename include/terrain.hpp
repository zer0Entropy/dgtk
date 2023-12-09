//
// Created by zeroc00l on 11/13/23.
//

#include <vector>
#include <nlohmann/json.hpp>
#include "position.hpp"

#ifndef DGTKPROJECT_TERRAIN_HPP
#define DGTKPROJECT_TERRAIN_HPP


enum class TerrainType {
    Empty = 0, Floor, Wall, TotalNumTerrainTypes
};

const std::vector<std::string> TerrainTypeNames = {
        {"empty"},
        {"floor"},
        {"wall"}
};

namespace sf {
    class Sprite;
    class Texture;
}

enum class TerrainPropertyID {
    Name = 0,
    TerrainType,
    Walkable,
    TexturePosition,
    TotalNumTerrainPropertyIDs
};

const std::vector<std::string> TerrainPropertyNames = {
        {"name"},
        {"type"},
        {"walkable"},
        {"texture_position"}
};

struct TerrainProperties {
    std::string                     name;
    TerrainType                     terrainType;
    bool                            isWalkable;
    bool                            isTransparent;
    Position                        texturePosition;
    sf::Texture*                    texture;
};

class Game;

TerrainProperties ReadTerrainPropertiesFromJSON(const nlohmann::json& jsonDoc, Game* game);
nlohmann::json WriteTerrainPropertiesToJSON(const TerrainProperties& terrainProperties);

#endif //DGTKPROJECT_TERRAIN_HPP
