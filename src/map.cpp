//
// Created by zeroc00l on 11/6/23.
//
#include <SFML/Graphics/Sprite.hpp>
#include "../include/map.hpp"

void GenerateMap(Map* map) {
    const MapProperties& properties(map->properties);
    const TilePlacementStrategy& strategy(properties.strategy);

    TerrainType terrainMap[properties.height][properties.width];

    for(int y = 0; y < properties.height; ++y) {
        for(int x = 0; x < properties.width; ++x) {
            if(y == 0) {
                terrainMap[y][x] = strategy.edges[(int)Direction::Up];
            } else if( y == properties.height - 1) {
                terrainMap[y][x] = strategy.edges[(int)Direction::Down];
            } else if( x == 0) {
                terrainMap[y][x] = strategy.edges[(int)Direction::Left];
            } else if( x == properties.width - 1) {
                terrainMap[y][x] = strategy.edges[(int)Direction::Right];
            } else {
                terrainMap[y][x] = strategy.defaultTerrainType;
            }
        }
    }

    for(int y = 0; y < properties.height; ++y) {
        for(int x = 0; x < properties.width; ++x) {
            Tile& tile( map->tileArray[y][x] );
            TerrainType terrainType = terrainMap[y][x];
            tile.terrain = *map->terrains.at(terrainType).get();
            tile.sprite.reset(new sf::Sprite);
            tile.creature = nullptr;
            tile.isVisible = true;
        }
    }
}
