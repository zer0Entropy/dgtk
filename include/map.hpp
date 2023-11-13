//
// Created by zeroc00l on 10/31/23.
//

#include <map>
#include <memory>
#include <string>
#include <vector>
#include <nlohmann/json.hpp>
#include "id.hpp"
#include "location.hpp"
#include "position.hpp"
#include "display.hpp"
#include "math.hpp"

#ifndef DGTKPROJECT_MAP_HPP
#define DGTKPROJECT_MAP_HPP

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
    Position                        texturePosition;
    sf::Texture*                    texture;
};

class Game;

TerrainProperties ReadTerrainPropertiesFromJSON(const nlohmann::json& jsonDoc, Game* game);
nlohmann::json WriteTerrainPropertiesToJSON(const TerrainProperties& terrainProperties);

class Creature;

struct Tile {
    TerrainProperties*              terrain;
    bool                            isVisible;
    std::unique_ptr<sf::Sprite>     sprite;
    Creature*                       creature;
};

struct Room {
    MapLocation topLeft;
    int width;
    int height;
};

struct TilePlacementStrategy {
    TerrainType     defaultTerrainType;
    TerrainType     edges[(int)Direction::TotalNumCardinalDirections];
};

constexpr int MaxMapWidth(100);
constexpr int MaxMapHeight(100);

enum class MapPropertyID {
    MapName = 0,
    Width,
    Height,
    TexturePath,
    TextureWidth,
    TextureHeight,
    TerrainProperties,
    TilePlacementStrategy,
    DefaultTerrain,
    NorthEdge,
    SouthEdge,
    WestEdge,
    EastEdge,
    TotalNumMapPropertyIDs
};

const std::vector<std::string> MapPropertyNames = {
        {"name"},
        {"width"},
        {"height"},
        {"texture_path"},
        {"texture_width"},
        {"texture_height"},
        {"terrain"},
        {"tile_placement"},
        {"default_terrain"},
        {"north_edge"},
        {"south_edge"},
        {"west_edge"},
        {"east_edge"}
};

struct MapProperties {
    std::string     name;
    int             width;
    int             height;
    std::string     texturePath;
    int             textureWidth;
    int             textureHeight;
    TilePlacementStrategy   strategy;
    std::map<TerrainType,TerrainProperties> terrainProperties;
    std::vector<Room>       roomList;
};

struct Map {
    MapProperties   properties;
    Tile            tileArray[MaxMapWidth][MaxMapHeight];
};

MapProperties ReadMapPropertiesFromJSON(const nlohmann::json& jsonDoc, Game* game);
nlohmann::json WriteMapPropertiesToJSON(const MapProperties& mapProperties);

void GenerateMap(Map* map, const DisplayConfig& displayConfig);

#endif //DGTKPROJECT_MAP_HPP
