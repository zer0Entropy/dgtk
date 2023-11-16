//
// Created by zeroc00l on 11/13/23.
//
#include "../include/dijkstra.hpp"

Dijkstra::DistanceMap::DistanceMap(LogSystem* logSystemPtr): logSystem(logSystemPtr) {

}

Dijkstra::Node* Dijkstra::DistanceMap::GetNode(int x, int y) {
    if(x >= 0 && x < width && y >= 0 && y < height) {
        return &nodeMap[y][x];
    }
    return nullptr;
}

void Dijkstra::DistanceMap::Generate(const MapLocation& originLocation, int mapWidth, int mapHeight) {
    origin = originLocation;
    width = mapWidth;
    height = mapHeight;
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            nodeMap[y][x].location = MapLocation{x, y};
        }
    }
    currentNode = GetNode(origin.x, origin.y);
    currentNode->distance = 0;
    Dijkstra::DistanceMap::PopulateFrontier();
    int nodeCount(1);
    do {
        PopulateFrontier();
        if(!frontier.empty()) {
            currentNode = frontier.front();
            frontier.pop_front();
            ++nodeCount;
        }
    } while(!frontier.empty());
}

Path Dijkstra::DistanceMap::FindPath(const MapLocation& destination) {

    Path path;
    currentNode = GetNode(destination.x, destination.y);
    path.steps.push_front(currentNode->location);
    path.steps.push_front(currentNode->location);
    while(currentNode->location.x != origin.x || currentNode->location.y != origin.y) {
        currentNode = currentNode->previous;
        path.steps.push_front(currentNode->location);
    }
    return path;
}

void Dijkstra::DistanceMap::PopulateFrontier() {
    if(!currentNode) { return; }
    MapLocation location(currentNode->location);

    if(location.x > 0) {
        Node& left(nodeMap[location.y][location.x - 1]);
        if(left.distance < 0) {
            left.previous = currentNode;
            left.distance = currentNode->distance + 1;
            frontier.push_back(&left);
        }
    } // LEFT

    if(location.x < width - 1) {
        Node& right(nodeMap[location.y][location.x + 1]);
        if(right.distance < 0) {
            right.previous = currentNode;
            right.distance = currentNode->distance + 1;
            frontier.push_back(&right);
        }
    } // RIGHT

    if(location.y > 0) {
        Node& up(nodeMap[location.y - 1][location.x]);
        if(up.distance < 0) {
            up.previous = currentNode;
            up.distance = currentNode->distance + 1;
            frontier.push_back(&up);
        }
    } // UP

    if(location.y < height - 1) {
        Node& down(nodeMap[location.y + 1][location.x]);
        if(down.distance < 0) {
            down.previous = currentNode;
            down.distance = currentNode->distance + 1;
            frontier.push_back(&down);
        }
    } // DOWN
}
