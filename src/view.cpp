//
// Created by zeroc00l on 11/1/23.
//

#include "../include/view.hpp"
#include "../include/game.hpp"

MapView CreateMapView(const nlohmann::json& mapViewJSON, Map *map, Game* game) {
    MapView mapView;
    MathParser& mathParser = game->GetMathParser();

    auto findWidth = mapViewJSON.find("view_width");
    auto findHeight = mapViewJSON.find("view_height");
    auto findPosition = mapViewJSON.find("position");

    if(findWidth != mapViewJSON.end()) {
        if(findWidth->is_string()) {
            Expression mathExpression;
            mathExpression.text = findWidth->get<std::string>();
            int result = mathParser.EvaluateExpression(mathExpression);
            mapView.widthInPixels = result;
        } else {
            mapView.widthInPixels = findWidth->get<int>();
        }
        mapView.widthInTiles = mapView.widthInPixels / (map->properties.textureWidth * game->GetDisplayConfig().tileScaleX);
    }
    if(findHeight != mapViewJSON.end()) {
        if(findHeight->is_string()) {
            Expression mathExpression;
            mathExpression.text = findHeight->get<std::string>();
            int result = mathParser.EvaluateExpression(mathExpression);
            mapView.heightInPixels = result;
        } else {
            mapView.heightInPixels = findWidth->get<int>();
        }
        mapView.heightInTiles = mapView.heightInPixels / (map->properties.textureHeight * game->GetDisplayConfig().tileScaleY);
    }
    if(findPosition != mapViewJSON.end()) {
        auto positionJSON = *findPosition;
        auto findX = positionJSON.find("x");
        auto findY = positionJSON.find("y");
        if(findX != positionJSON.end()) {
            if(findX->is_string()) {
                Expression mathExpression;
                mathExpression.text = findX->get<std::string>();
                int result = mathParser.EvaluateExpression(mathExpression);
                mapView.position.x = result;
            } else {
                mapView.position.x = findWidth->get<int>();
            }
        }
        if(findY != positionJSON.end()) {
            if(findY->is_string()) {
                Expression mathExpression;
                mathExpression.text = findHeight->get<std::string>();
                int result = mathParser.EvaluateExpression(mathExpression);
                mapView.position.y = result;
            } else {
                mapView.position.y = findWidth->get<int>();
            }
        }
    }

    return mapView;
}

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
