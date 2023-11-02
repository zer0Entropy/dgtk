//
// Created by zeroc00l on 11/1/23.
//

#include "../include/view.hpp"

bool ShiftMapView(MapView& view, const Map& map, const MapLocation& playerLocation, Direction direction) {
    bool success(false);
    MapLocation center(view.centerLocation);
    int mapWidth(map.properties.width / map.properties.textureWidth);
    int mapHeight(map.properties.height / map.properties.textureHeight);
    switch(direction) {
        case Direction::Up:
            if(playerLocation.y > center.y && center.y > view.heightInTiles / 2) {
                view.centerLocation.y--;
                success = true;
            }
            break;
        case Direction::Down:
            if(playerLocation.y > center.y && center.y < mapHeight - (view.heightInTiles / 2)) {
                view.centerLocation.y++;
                success = true;
            }
            break;
        case Direction::Left:
            if(playerLocation.x < center.x && center.x > view.widthInTiles / 2) {
                view.centerLocation.x--;
                success = true;
            }
            break;
        case Direction::Right:
            if(playerLocation.x > center.x && center.x < mapWidth - (view.widthInTiles / 2)) {
                view.centerLocation.x++;
                success = true;
            }
            break;
        case Direction::None:   default:    break;
    }
    return success;
}
