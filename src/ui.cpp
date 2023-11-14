//
// Created by zeroc00l on 11/6/23.
//
#include "../include/ui.hpp"

uiObjectProperties ReadUIObjPropertiesFromJSON(const nlohmann::json& jsonDoc, MathParser& mathParser) {
    uiObjectProperties uiObjProperties;

    for(auto uiProperty = jsonDoc.begin(); uiProperty != jsonDoc.end(); ++uiProperty) {
        std::string_view key( uiProperty.key() );
        auto value( uiProperty.value() );
        if(key.compare(uiPropertyNames.at((int)uiPropertyID::ObjectID)) == 0) {
            uiObjProperties.id = value.get<std::string>();
        } else if(key.compare(uiPropertyNames.at((int)uiPropertyID::ObjectType)) == 0) {
            std::string objTypeString = value.get<std::string>();
            if(objTypeString.compare("decoration") == 0) {
                uiObjProperties.uiType = uiObjectType::Decoration;
            } else if(objTypeString.compare("toolbar") == 0) {
                uiObjProperties.uiType = uiObjectType::Toolbar;
            } else if(objTypeString.compare("scrollbar") == 0) {
                uiObjProperties.uiType = uiObjectType::Scrollbar;
            } else if(objTypeString.compare("map_view") == 0) {
                uiObjProperties.uiType = uiObjectType::MapView;
            } else if(objTypeString.compare("overlay") == 0) {
                uiObjProperties.uiType = uiObjectType::Overlay;
            }
        } else if(key.compare(uiPropertyNames.at((int)uiPropertyID::Alignment)) == 0) {
            std::string alignString = value.get<std::string>();
            if(alignString.compare("left") == 0) {
                uiObjProperties.align = Alignment::Left;
            } else if(alignString.compare("center") == 0) {
                uiObjProperties.align = Alignment::Center;
            } else if(alignString.compare("right") == 0) {
                uiObjProperties.align = Alignment::Right;
            }
        } else if(key.compare(uiPropertyNames.at((int)uiPropertyID::Origin)) == 0) {
            auto originJSON = value;
            auto findX = originJSON.find("x");
            auto findY = originJSON.find("y");
            if(findX != originJSON.end()) {
                if(findX->is_string()) {
                    Expression mathExpression;
                    mathExpression.text = findX->get<std::string>();
                    int result = mathParser.EvaluateExpression(mathExpression);
                    uiObjProperties.origin.x = result;
                } else {
                    uiObjProperties.origin.x = findX->get<int>();
                }
                mathParser.RegisterVariable("origin.x", uiObjProperties.origin.x);
            }
            if(findY != originJSON.end()) {
                if(findY->is_string()) {
                    Expression mathExpression;
                    mathExpression.text = findY->get<std::string>();
                    int result = mathParser.EvaluateExpression(mathExpression);
                    uiObjProperties.origin.y = result;
                } else {
                    uiObjProperties.origin.y = findY->get<int>();
                }
                mathParser.RegisterVariable("origin.y", uiObjProperties.origin.y);
            }
        }  else if(key.compare(uiPropertyNames.at((int)uiPropertyID::Position)) == 0) {
            auto positionJSON = value;
            auto findX = positionJSON.find("x");
            auto findY = positionJSON.find("y");
            if(findX != positionJSON.end()) {
                if(findX->is_string()) {
                    Expression mathExpression;
                    mathExpression.text = findX->get<std::string>();
                    int result = mathParser.EvaluateExpression(mathExpression);
                    uiObjProperties.position.x = result;
                } else {
                    uiObjProperties.position.x = findX->get<int>();
                }
            }
            if(findY != positionJSON.end()) {
                if(findY->is_string()) {
                    Expression mathExpression;
                    mathExpression.text = findY->get<std::string>();
                    int result = mathParser.EvaluateExpression(mathExpression);
                    uiObjProperties.position.y = result;
                } else {
                    uiObjProperties.position.y = findY->get<int>();
                }
            }
        } else if(key.compare(uiPropertyNames.at((int)uiPropertyID::LayerID)) == 0) {
            std::string layerString = value.get<std::string>();
            if(layerString.compare("background") == 0) {
                uiObjProperties.layer = LayerID::Background;
            } else if(layerString.compare("window_frame") == 0) {
                uiObjProperties.layer = LayerID::WindowFrame;
            } else if(layerString.compare("sprite") == 0) {
                uiObjProperties.layer = LayerID::Sprite;
            } else if(layerString.compare("text") == 0) {
                uiObjProperties.layer = LayerID::Text;
            } else if(layerString.compare("overlay") == 0) {
                uiObjProperties.layer = LayerID::Overlay;
            }
        }  else if(key.compare(uiPropertyNames.at((int)uiPropertyID::TexturePath)) == 0) {
            uiObjProperties.textureSource.pathToFile = value.get<std::string>();
        }  else if(key.compare(uiPropertyNames.at((int)uiPropertyID::TexturePosition)) == 0) {
            auto positionJSON = value;
            auto findX = positionJSON.find("x");
            auto findY = positionJSON.find("y");
            if(findX != positionJSON.end()) {
                uiObjProperties.textureSource.topLeft.x = findX->get<int>();
            }
            if(findY != positionJSON.end()) {
                uiObjProperties.textureSource.topLeft.y = findY->get<int>();
            }
        }  else if(key.compare(uiPropertyNames.at((int)uiPropertyID::TextureWidth)) == 0) {
            uiObjProperties.textureSource.width = value.get<int>();
        } else if(key.compare(uiPropertyNames.at((int)uiPropertyID::TextureHeight)) == 0) {
            uiObjProperties.textureSource.height = value.get<int>();
        }
    }

    return uiObjProperties;
}

nlohmann::json WriteUIObjPropertiesToJSON(const uiObjectProperties& uiObjProperties) {
    nlohmann::json jsonProperties;
    // objectID
    jsonProperties[uiPropertyNames.at((int)uiPropertyID::ObjectID)] = uiObjProperties.id;
    // objectType
    jsonProperties[uiPropertyNames.at((int)uiPropertyID::ObjectType)] = uiObjProperties.uiType;
    // alignment
    jsonProperties[uiPropertyNames.at((int)uiPropertyID::Alignment)] = uiObjProperties.align;
    // position
    jsonProperties[uiPropertyNames.at((int)uiPropertyID::Position)] = nlohmann::json{
            {"x", uiObjProperties.position.x}, {"y", uiObjProperties.position.y}
    };
    // layerID
    jsonProperties[uiPropertyNames.at((int)uiPropertyID::LayerID)] = uiObjProperties.layer;
    // texturePath
    jsonProperties[uiPropertyNames.at((int)uiPropertyID::TexturePath)] = uiObjProperties.textureSource.pathToFile;
    // texturePosition
    jsonProperties[uiPropertyNames.at((int)uiPropertyID::TexturePosition)] = nlohmann::json{
            {"x", uiObjProperties.textureSource.topLeft.x}, {"y", uiObjProperties.textureSource.topLeft.y}
    };
    // textureWidth
    jsonProperties[uiPropertyNames.at((int)uiPropertyID::TextureWidth)] = uiObjProperties.textureSource.width;
    // textureHeight
    jsonProperties[uiPropertyNames.at((int)uiPropertyID::TextureHeight)] = uiObjProperties.textureSource.height;
    return jsonProperties;
}
