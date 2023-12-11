//
// Created by zeroc00l on 11/1/23.
//

#include "../include/view.hpp"
#include "../include/game.hpp"

MapViewProperties ReadMapViewPropertiesFromJSON(const nlohmann::json& mapViewJSON, Game* game) {
    MapViewProperties viewProperties;

    MathParser& mathParser = game->GetMathParser();

    auto findWidth = mapViewJSON.find("view_width");
    auto findHeight = mapViewJSON.find("view_height");
    auto findPosition = mapViewJSON.find("position");

    if(findWidth != mapViewJSON.end()) {
        if(findWidth->is_string()) {
            Expression mathExpression;
            mathExpression.text = findWidth->get<std::string>();
            int result = mathParser.EvaluateExpression(mathExpression);
            viewProperties.widthInPixels = result;
        } else {
            viewProperties.widthInPixels = findWidth->get<int>();
        }
    }
    if(findHeight != mapViewJSON.end()) {
        if(findHeight->is_string()) {
            Expression mathExpression;
            mathExpression.text = findHeight->get<std::string>();
            int result = mathParser.EvaluateExpression(mathExpression);
            viewProperties.heightInPixels = result;
        } else {
            viewProperties.heightInPixels = findHeight->get<int>();
        }
    }
    if(findPosition != mapViewJSON.end()) {
        auto positionJSON = *findPosition;
        auto findX = positionJSON.find("x");
        auto findY = positionJSON.find("y");
        if (findX != positionJSON.end()) {
            if (findX->is_string()) {
                Expression mathExpression;
                mathExpression.text = findX->get<std::string>();
                int result = mathParser.EvaluateExpression(mathExpression);
                viewProperties.position.x = result;
            } else {
                viewProperties.position.x = findX->get<int>();
            }
        }
        if(findY != positionJSON.end()) {
            if(findY->is_string()) {
                Expression mathExpression;
                mathExpression.text = findY->get<std::string>();
                int result = mathParser.EvaluateExpression(mathExpression);
                viewProperties.position.y = result;
            } else {
                viewProperties.position.y = findY->get<int>();
            }
        }
    }

    return viewProperties;
}

void CenterViewOnPlayer(MapView& view, const Map& map, MapLocation playerLocation) {
    MapLocation center(playerLocation);
    if(playerLocation.x < view.properties.widthInTiles / 2) {
        center.x = view.properties.widthInTiles / 2;
    } else if(playerLocation.x >= map.properties.width - (view.properties.widthInTiles / 2)) {
        center.x = map.properties.width - (view.properties.widthInTiles / 2) - 1;
    }
    if(playerLocation.y < view.properties.heightInTiles / 2) {
        center.y = view.properties.heightInTiles / 2;
    } else if(playerLocation.y >= (map.properties.height - (view.properties.heightInTiles / 2)) - 1) {
        center.y = map.properties.height - (view.properties.heightInTiles / 2) - 1;
    }
    view.properties.centerLocation = center;
}

void InitVisibilityMap(const Map& map, Dijkstra::WeightedDistanceMap& visibilityMap) {
    visibilityMap.InitWeightsByVisibility(map);
}

void UpdateVisibilityMap(Map& map, Dijkstra::WeightedDistanceMap& visibilityMap, const MapLocation& center) {
    visibilityMap.Generate(center, map.properties.width, map.properties.height);
    for(int y = 0; y < map.properties.height; ++y) {
        for(int x = 0; x < map.properties.width; ++x) {
            Tile& tile(map.tileArray[y][x]);
            int visibility = visibilityMap.GetNode(x, y)->distance;
            if( visibility <= VisibilityThreshold ||
                (visibility >= Dijkstra::WallWeight && visibility - Dijkstra::WallWeight <= VisibilityThreshold) ) {
                tile.isVisible = true;
            } else {
                tile.isVisible = false;
            }
        }
    }
    MapArea* currentArea = FindArea(map, center);
    if(currentArea->height > 0 && currentArea->width > 0) {
        for(int y = currentArea->topLeft.y; y < currentArea->topLeft.y + currentArea->height; ++y) {
            for(int x = currentArea->topLeft.x; x < currentArea->topLeft.x + currentArea->width; ++x) {
                Tile& tile(map.tileArray[y][x]);
                tile.isVisible = true;
            }
        }
    } else {
        Hallway* hallway = static_cast<Hallway*>(currentArea);
        for(const auto& step : hallway->path.steps) {
            Tile& tile(map.tileArray[step.y][step.x]);
            tile.isVisible = true;
        }
    }
}
