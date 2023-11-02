//
// Created by zeroc00l on 11/1/23.
//

#include "../include/view.hpp"

void CenterViewOnPlayer(MapView& view, const Map& map, MapLocation playerLocation) {
    MapLocation center(playerLocation);
    if(playerLocation.x < view.widthInTiles / 2) {
        center.x = view.widthInTiles / 2;
    } else if(playerLocation.x >= map.properties.width - (view.widthInTiles / 2)) {
        center.x = map.properties.width - (view.widthInTiles / 2);
    }
    if(playerLocation.y < view.heightInTiles / 2) {
        center.y = view.heightInTiles / 2;
    } else if(playerLocation.y >= map.properties.height - (view.heightInTiles / 2)) {
        center.y = map.properties.height - (view.heightInTiles / 2);
    }
    view.centerLocation = center;
}
