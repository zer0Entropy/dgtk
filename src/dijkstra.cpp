//
// Created by zeroc00l on 11/13/23.
//
#include "../include/dijkstra.hpp"
#include "../include/map.hpp"

Dijkstra::DistanceMap::DistanceMap() {

}

const MapLocation& Dijkstra::DistanceMap::GetOrigin() const {
    return origin;
}

Dijkstra::Node* Dijkstra::DistanceMap::GetNode(int x, int y) const {
    if(x >= 0 && x < width && y >= 0 && y < height) {
        return (Dijkstra::Node*)&nodeMap[y][x];
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
            nodeMap[y][x].distance = -1;
            nodeMap[y][x].previous = nullptr;
        }
    }
    currentNode = GetNode(origin.x, origin.y);
    currentNode->distance = 0;
    PopulateFrontier();
    do {
        PopulateFrontier();
        if(!frontier.empty()) {
            currentNode = frontier.front();
            frontier.pop_front();
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

void Dijkstra::WeightedDistanceMap::InitWeightsByWalkability(const Map& map) {
    for(int y = 0; y < map.properties.height; ++y) {
        for(int x = 0; x < map.properties.width; ++x) {
            const Tile& tile(map.tileArray[y][x]);
            if(tile.terrain->isWalkable) {
                nodeWeights[y][x] = FloorWeight;
            } else {
                nodeWeights[y][x] = WallWeight;
            }
        } // x
    } // y
}

void Dijkstra::WeightedDistanceMap::InitWeightsByVisibility(const Map& map) {
    for(int y = 0; y < map.properties.height; ++y) {
        for(int x = 0; x < map.properties.width; ++x) {
            const Tile& tile(map.tileArray[y][x]);
            if(tile.terrain->isTransparent) {
                nodeWeights[y][x] = FloorWeight;
            } else {
                nodeWeights[y][x] = WallWeight;
            }
        } // x
    } // y
}

void Dijkstra::WeightedDistanceMap::PopulateFrontier() {
    if(!currentNode) { return; }
    MapLocation location(currentNode->location);

    if(location.x > 0) {
        Node& left(nodeMap[location.y][location.x - 1]);
        if(left.distance < 0) {
            left.previous = currentNode;
            left.distance = currentNode->distance + nodeWeights[left.location.y][left.location.x];
            frontier.push_back(&left);
        }
    } // LEFT

    if(location.x < width - 1) {
        Node& right(nodeMap[location.y][location.x + 1]);
        if(right.distance < 0) {
            right.previous = currentNode;
            right.distance = currentNode->distance + nodeWeights[right.location.y][right.location.x];
            frontier.push_back(&right);
        }
    } // RIGHT

    if(location.y > 0) {
        Node& up(nodeMap[location.y - 1][location.x]);
        if(up.distance < 0) {
            up.previous = currentNode;
            up.distance = currentNode->distance + nodeWeights[up.location.y][up.location.x];
            frontier.push_back(&up);
        }
    } // UP

    if(location.y < height - 1) {
        Node& down(nodeMap[location.y + 1][location.x]);
        if(down.distance < 0) {
            down.previous = currentNode;
            down.distance = currentNode->distance + nodeWeights[down.location.y][down.location.x];
            frontier.push_back(&down);
        }
    } // DOWN
}
