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

struct MapArea {
    MapLocation topLeft;
    int width;
    int height;

    MapArea():
        topLeft{0, 0}, width(0), height(0) {}
    MapArea(const MapLocation& origin, int hSize, int vSize):
        topLeft(origin), width(hSize), height(vSize) {}
    MapArea(const MapArea& copy) = default;
    ~MapArea() = default;
};

struct Room: public MapArea {
    UniqueID id;
    MapLocation center;

    Room(): MapArea(),
        id(""), center{0, 0} {}
    Room(const MapLocation& origin, int hSize, int vSize, UniqueID roomID, const MapLocation& centerLocation):
        MapArea(origin, hSize, vSize), id(roomID), center(centerLocation) {}
    Room(const Room& copy) = default;
    ~Room() = default;
};

struct Hallway: public MapArea {
    Room* origin;
    Room* destination;
    Path path;

    Hallway(): MapArea(),
        origin(nullptr), destination(nullptr) {}
    Hallway(Room* originRoom, Room* destRoom, const Path& pathTraveled):
        MapArea(), origin(originRoom), destination(destRoom), path(pathTraveled) {}
    Hallway(const Hallway& copy) = default;
    ~Hallway() = default;
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
    std::string                             name;
    int                                     width;
    int                                     height;
    std::string                             texturePath;
    int                                     textureWidth;
    int                                     textureHeight;
    TilePlacementStrategy                   strategy;
    std::map<TerrainType,TerrainProperties> terrainProperties;
    std::map<UniqueID, Room>                roomList;
    std::vector<Hallway>                    hallwayList;
};

struct Map {
    MapProperties   properties;
    Tile            tileArray[MaxMapWidth][MaxMapHeight];
};

MapProperties ReadMapPropertiesFromJSON(const nlohmann::json& jsonDoc, Game* game);
nlohmann::json WriteMapPropertiesToJSON(const MapProperties& mapProperties);

void GenerateMap(Map& map, RandomNumberGenerator& rng, const DisplayConfig& displayConfig);

namespace Dijkstra {
    class DistanceMap;
};

Hallway CreateHallway(Map& map, Dijkstra::DistanceMap& distanceMap, const Room& origin, const Room& destination);

Room FindCenterRoom(const Map& map);

Room GetRoom(const Map& map, UniqueID roomID);

MapArea* FindArea(const Map& map, const MapLocation& location);

bool IsInRoom(const MapLocation& location, const Room& room);

bool IsInHallway(const MapLocation& location, const Hallway& hallway);

#endif //DGTKPROJECT_MAP_HPP
