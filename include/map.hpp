//
// Created by zeroc00l on 10/31/23.
//

#include <map>
#include <memory>
#include <string>

#include "id.hpp"
#include "location.hpp"
#include "position.hpp"
#include "display.hpp"
#include "math.hpp"
#include "path.hpp"
#include "terrain.hpp"
#include "log.hpp"

#ifndef DGTKPROJECT_MAP_HPP
#define DGTKPROJECT_MAP_HPP

constexpr int MaxMapWidth(100);
constexpr int MaxMapHeight(100);

class Creature;

struct Tile {
    TerrainProperties*              terrain;
    bool                            isVisible;
    std::unique_ptr<sf::Sprite>     sprite;
    Creature*                       creature;
};

struct TilePlacementStrategy {
    TerrainType     defaultTerrainType;
    TerrainType     edges[(int)Direction::TotalNumCardinalDirections];
};

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
    std::vector<Room>       hallwayList;
};

struct Map {
    MapProperties   properties;
    Tile            tileArray[MaxMapWidth][MaxMapHeight];
};

MapProperties ReadMapPropertiesFromJSON(const nlohmann::json& jsonDoc, Game* game);
nlohmann::json WriteMapPropertiesToJSON(const MapProperties& mapProperties);

void GenerateMap(Map* map, const DisplayConfig& displayConfig);

void CreateHallways(Map& map, LogSystem* logSystem);

#endif //DGTKPROJECT_MAP_HPP
