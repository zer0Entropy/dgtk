//
// Created by zeroc00l on 10/31/23.
//

#include <map>
#include <memory>
#include <string>
#include "location.hpp"
#include "position.hpp"

#ifndef DGTKPROJECT_MAP_HPP
#define DGTKPROJECT_MAP_HPP

enum class TerrainType {
    Empty = 0, Floor, Wall
};

namespace sf {
    class Sprite;
    class Texture;
}

struct Terrain {
    std::string                     name;
    TerrainType                     terrainType;
    bool                            isWalkable;
    Position                        texturePosition;
    sf::Texture*                    texture;
};

class Creature;

struct Tile {
    Terrain*                        terrain;
    bool                            isVisible;
    std::unique_ptr<sf::Sprite>     sprite;
    Creature*                       creature;
};

struct TilePlacementStrategy {
    TerrainType     defaultTerrainType;
    TerrainType     edges[(int)Direction::TotalNumCardinalDirections];
};

constexpr int MaxMapWidth(100);
constexpr int MaxMapHeight(100);

struct MapProperties {
    int             width;
    int             height;
    std::string     texturePath;
    int             textureWidth;
    int             textureHeight;
    TilePlacementStrategy   strategy;
};

struct Map {
    MapProperties   properties;
    Tile            tileArray[MaxMapWidth][MaxMapHeight];
    std::map<TerrainType,std::unique_ptr<Terrain*>> terrains;
};

void GenerateMap(Map* map);

#endif //DGTKPROJECT_MAP_HPP
