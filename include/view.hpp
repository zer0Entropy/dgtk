//
// Created by zeroc00l on 11/1/23.
//

#include "map.hpp"

#ifndef DGTKPROJECT_VIEW_HPP
#define DGTKPROJECT_VIEW_HPP

struct MapView {
    int widthInPixels, heightInPixels;
    int widthInTiles, heightInTiles;
    MapLocation centerLocation;
};

bool ShiftMapView(MapView& view, const Map& map, const MapLocation& playerLocation, Direction direction);


#endif //DGTKPROJECT_VIEW_HPP
