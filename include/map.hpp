//
// Created by zeroc00l on 10/31/23.
//

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
}

class Creature;

struct Tile {
    TerrainType                     terrainType;
    bool                            isWalkable;
    bool                            isVisible;
    std::unique_ptr<sf::Sprite>     sprite;
    Creature*                       creature;
};

constexpr int MaxMapWidth(100);
constexpr int MaxMapHeight(100);

struct MapProperties {
    int             width;
    int             height;
    std::string     texturePath;
    int             textureWidth;
    int             textureHeight;
    Position        wallTexturePos;
    Position        floorTexturePos;
};

struct Map {
    MapProperties   properties;
    Tile            tileArray[MaxMapWidth][MaxMapHeight];
};

#endif //DGTKPROJECT_MAP_HPP
