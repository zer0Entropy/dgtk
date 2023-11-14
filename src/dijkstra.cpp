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

void Dijkstra::DistanceMap::Generate(const MapLocation& originLocation, const Map& source) {
    std::string message("Generating Dijkstra map...");
    logSystem->PublishMessage(message);
    message = "Origin: (";
    message.append(std::to_string(originLocation.x));
    message.append(", ");
    message.append(std::to_string(originLocation.y));
    message.append(")");
    logSystem->PublishMessage(message);

    origin = originLocation;
    width = source.properties.width;
    height = source.properties.height;
    for(int y = 0; y < height; ++y) {
        for(int x = 0; x < width; ++x) {
            nodeMap[y][x].location = MapLocation{x, y};
        }
    }
    currentNode = GetNode(origin.x, origin.y);
    Dijkstra::DistanceMap::PopulateFrontier();
    while(!frontier.empty()) {
        do {
            currentNode = frontier.front();
            frontier.pop_front();
        } while(!frontier.empty() && currentNode->distance >= 0);
        PopulateFrontier();
    }
}

Path Dijkstra::DistanceMap::FindPath(const MapLocation& destination) {
    std::string message("Finding path to destination: (");
    message.append(std::to_string(destination.x));
    message.append(", ");
    message.append(std::to_string(destination.y));
    message.append(")'");

    Path path;
    currentNode = GetNode(destination.x, destination.y);
    path.steps.push_front(destination);
    while(currentNode->location.x != origin.x || currentNode->location.y != origin.y) {
        PopulateAdjacent();
        while (!adjacent.empty()) {
            Node* adjNode = adjacent.front();
            adjacent.pop_front();
            if (adjNode->distance < currentNode->distance) {
                currentNode = adjNode;
                path.steps.push_front(currentNode->location);
            }
        }
    }

    message = "Path found successfully - [";
    message.append(std::to_string(path.steps.size()));
    message.append(" steps]");
    logSystem->PublishMessage(message);
    return path;
}

void Dijkstra::DistanceMap::PopulateFrontier() {
    if(!currentNode) { return; }
    // If currentNode->previous == nullptr, mark as origin
    if(!currentNode->previous && currentNode->distance < 0) {
        currentNode->distance = 0;
    } else {
        if (currentNode->previous) {
            currentNode->distance = currentNode->previous->distance + 1;
        }
    }
    PopulateAdjacent();
    while(!adjacent.empty()) {
        Node* adjNode(adjacent.front());
        adjacent.pop_front();
        if(adjNode->distance < 0) {
            adjNode->previous = currentNode;
            frontier.push_back(adjNode);
        }
    }
}

void Dijkstra::DistanceMap::PopulateAdjacent() {
    if(!currentNode) { return; }
    MapLocation location(currentNode->location);
    // LOOK LEFT
    if(location.x > 0) {
        Node* left = &nodeMap[location.y][location.x - 1];
        adjacent.push_back(left);
    } // LOOK LEFT

    // LOOK RIGHT
    if(location.x < width - 1) {
        Node* right = &nodeMap[location.y][location.x + 1];
        adjacent.push_back(right);
    } // LOOK RIGHT

    // LOOK UP
    if(location.y > 0) {
        Node* up = &nodeMap[location.y - 1][location.x];
        adjacent.push_back(up);
    } // LOOK UP

    // LOOK DOWN
    if(location.y < height - 1) {
        Node* down = &nodeMap[location.y + 1][location.x];
        adjacent.push_back(down);
    } // LOOK DOWN
}
