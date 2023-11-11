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
            viewProperties.heightInPixels = findWidth->get<int>();
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
        center.x = map.properties.width - (view.properties.widthInTiles / 2);
    }
    if(playerLocation.y < view.properties.heightInTiles / 2) {
        center.y = view.properties.heightInTiles / 2;
    } else if(playerLocation.y >= map.properties.height - (view.properties.heightInTiles / 2)) {
        center.y = map.properties.height - (view.properties.heightInTiles / 2);
    }
    view.properties.centerLocation = center;
}
