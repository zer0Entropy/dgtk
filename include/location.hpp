//
// Created by zeroc00l on 11/1/23.
//

#ifndef DGTKPROJECT_LOCATION_HPP
#define DGTKPROJECT_LOCATION_HPP

enum class Direction { None = 0, Up, Down, Left, Right, TotalNumCardinalDirections };
enum class Diagonal { UpLeft, UpRight, DownLeft, DownRight, TotalNumDiagonals };

struct MapLocation {
    int x;
    int y;
};

struct Room {
    MapLocation topLeft;
    MapLocation center;
    int width;
    int height;
    bool isConnected;
};

#endif //DGTKPROJECT_LOCATION_HPP
