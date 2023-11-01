//
// Created by zeroc00l on 10/31/23.
//

#ifndef DGTKPROJECT_MAP_HPP
#define DGTKPROJECT_MAP_HPP

enum class TerrainType {
    Empty = 0, Floor, Wall
};

struct Tile {
    TerrainType     terrainType;
    bool            isWalkable;
    bool            isVisible;
};

constexpr int MaxMapWidth(100);
constexpr int MaxMapHeight(100);

struct Map {
    int             width;
    int             height;
    Tile            tileArray[MaxMapWidth][MaxMapHeight];
};

#endif //DGTKPROJECT_MAP_HPP
