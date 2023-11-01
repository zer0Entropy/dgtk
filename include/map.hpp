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
