//
// Created by zeroc00l on 11/1/23.
//
#include <fstream>
#include <SFML/Window/Event.hpp>
#include "../include/scene.hpp"
#include "../include/game.hpp"
#include "../include/log.hpp"
#include "../include/math.hpp"

SceneID GetSceneID(UniqueID sceneName) {
    SceneID sceneID(SceneID::MainMenu);
    for(int index = 0; index < (int)SceneID::TotalNumSceneIDs; ++index) {
        std::string name(sceneName);
        if(name.compare(SceneNames.at((SceneID)index)) == 0) {
            sceneID = (SceneID)index;
        }
    }
    return sceneID;
}

Scene* LoadScene(std::string_view pathToJSON, Game* game) {
    Scene* scene(nullptr);

    const DisplayConfig& displayConfig(game->GetDisplayConfig());
    int winHeight(displayConfig.windowProperties.height - displayConfig.windowHeightModifier);
    MathParser mathParser;
    mathParser.RegisterVariable("window_width", displayConfig.windowProperties.width);
    mathParser.RegisterVariable("window_height", displayConfig.windowProperties.height - displayConfig.windowHeightModifier);

    nlohmann::json jsonDoc;
    std::ifstream jsonFile(std::string{pathToJSON});
    if(!jsonFile.good()) {
        // TODO: Report error here!
        return scene;
    } else {
        jsonDoc = nlohmann::json::parse(jsonFile);
    }

    LogSystem* logSystem = game->GetLogSystem();

    scene = new Scene;
    for(auto scenePropertyIter = jsonDoc.begin(); scenePropertyIter != jsonDoc.end(); ++scenePropertyIter) {
        for(int sceneKeyIndex = 0; sceneKeyIndex < (int)SceneKeyID::TotalNumSceneKeys; ++sceneKeyIndex) {
            SceneKeyID keyID((SceneKeyID)sceneKeyIndex);
            std::string_view key(SceneKeys.at(keyID));
            auto findKey = scenePropertyIter->find(key);
            if(findKey != scenePropertyIter->end()) {
                if(keyID == SceneKeyID::SceneID) {
                    scene->id = findKey->get<std::string>();
                }
                else if(keyID == SceneKeyID::UIObjects) {
                    const nlohmann::json& uiObjectsJSON(*findKey);
                    uiObject* uiObjectPtr(nullptr);
                    Decoration* decorationPtr(nullptr);
                    for(auto& uiObj : uiObjectsJSON) {
                        UniqueID uiObjID("");
                        uiObjectType uiObjType;
                        for(int propertyKeyIndex = (int)SceneKeyID::UIObjectID; propertyKeyIndex < (int)SceneKeyID::TotalNumSceneKeys; ++propertyKeyIndex) {
                            SceneKeyID propertyKeyID((SceneKeyID)propertyKeyIndex);
                            std::string_view propertyKey(SceneKeys.at(propertyKeyID));
                            auto findPropertyKey = uiObj.find(propertyKey);

                            if(findPropertyKey != uiObj.end()) {
                                if (propertyKeyIndex == (int) SceneKeyID::UIObjectID) {
                                    uiObjID = findPropertyKey->get<std::string>();
                                } else if(propertyKeyIndex == (int)SceneKeyID::UIType) {
                                    std::string uiObjTypeString(findPropertyKey->get<std::string>());
                                    if(uiObjTypeString.compare("Decoration") == 0) {
                                        uiObjType = uiObjectType::Decoration;
                                    } else if(uiObjTypeString.compare("Scrollbar") == 0) {
                                        uiObjType = uiObjectType::Scrollbar;
                                    } else if(uiObjTypeString.compare("Toolbar") == 0) {
                                        uiObjType = uiObjectType::Toolbar;
                                    }
                                } else if(propertyKeyIndex == (int)SceneKeyID::DecorationType) {
                                    std::string decTypeString(findPropertyKey->get<std::string>());
                                    DecorationType decType;
                                    if(decTypeString.compare("Background") == 0) {
                                        decType = DecorationType::Background;
                                    } else if(decTypeString.compare("Frame") == 0) {
                                        decType = DecorationType::Frame;
                                    } else if(decTypeString.compare("Doodad") == 0) {
                                        decType = DecorationType::Doodad;
                                    } else if(decTypeString.compare("Text") == 0) {
                                        decType = DecorationType::Text;
                                    }
                                    decorationPtr = game->CreateDecoration(uiObjID, decType);
                                    uiObjectPtr = decorationPtr;
                                } else if(propertyKeyIndex == (int)SceneKeyID::TexturePath) {
                                    uiObjectPtr->uiProperties.textureSource.pathToFile = findPropertyKey->get<std::string>();
                                } else if(propertyKeyIndex == (int)SceneKeyID::TextureWidth) {
                                    uiObjectPtr->uiProperties.textureSource.width = findPropertyKey->get<int>();
                                } else if(propertyKeyIndex == (int)SceneKeyID::TextureHeight) {
                                    uiObjectPtr->uiProperties.textureSource.height = findPropertyKey->get<int>();
                                } else if(propertyKeyIndex == (int)SceneKeyID::TexturePosition) {
                                    auto findTexturePosition = *findPropertyKey;
                                    auto findX = findTexturePosition.find("x");
                                    auto findY = findTexturePosition.find("y");
                                    uiObjectPtr->uiProperties.textureSource.topLeft = {
                                            findX->get<int>(),
                                            findY->get<int>()
                                    };
                                } else if(propertyKeyIndex == (int)SceneKeyID::Origin) {
                                    auto findOrigin = *findPropertyKey;
                                    auto findX = findOrigin.find("x");
                                    auto findY = findOrigin.find("y");
                                    if(findX->is_string()) {
                                        Expression expression;
                                        expression.text = findX->get<std::string>();
                                        int result = mathParser.EvaluateExpression(expression);
                                        uiObjectPtr->uiProperties.origin.x = result;
                                    } else {
                                        uiObjectPtr->uiProperties.origin.x = findX->get<int>();
                                    }
                                    if(findY->is_string()) {
                                        Expression expression;
                                        expression.text = findY->get<std::string>();
                                        int result = mathParser.EvaluateExpression(expression);
                                        uiObjectPtr->uiProperties.origin.y = result;
                                    } else {
                                        uiObjectPtr->uiProperties.origin.y = findY->get<int>();
                                    }
                                    mathParser.RegisterVariable("origin.x", uiObjectPtr->uiProperties.origin.x);
                                    mathParser.RegisterVariable("origin.y", uiObjectPtr->uiProperties.origin.y);
                                } else if(propertyKeyIndex == (int)SceneKeyID::Position) {
                                    auto findPosition = *findPropertyKey;
                                    auto findX = findPosition.find("x");
                                    auto findY = findPosition.find("y");
                                    if(findX->is_string()) {
                                        Expression expression;
                                        expression.text = findX->get<std::string>();
                                        int result = mathParser.EvaluateExpression(expression);
                                        uiObjectPtr->uiProperties.position.x = result;
                                    } else {
                                        uiObjectPtr->uiProperties.position.x = findX->get<int>();
                                    }
                                    if(findY->is_string()) {
                                        Expression expression;
                                        expression.text = findY->get<std::string>();
                                        int result = mathParser.EvaluateExpression(expression);
                                        uiObjectPtr->uiProperties.position.y = result;
                                    } else {
                                        uiObjectPtr->uiProperties.position.y = findY->get<int>();
                                    }
                                } else if(propertyKeyIndex == (int)SceneKeyID::Alignment) {
                                    std::string alignString(findPropertyKey->get<std::string>());
                                    if(alignString == "Left") {
                                        decorationPtr->uiProperties.align = Alignment::Left;
                                    } else if(alignString == "Center") {
                                        decorationPtr->uiProperties.align = Alignment::Center;
                                    } else if(alignString == "Right") {
                                        decorationPtr->uiProperties.align = Alignment::Right;
                                    }
                                } else if(propertyKeyIndex == (int)SceneKeyID::LayerID) {
                                    std::string layerID(findPropertyKey->get<std::string>());
                                    if(layerID.compare("background") == 0) {
                                        decorationPtr->uiProperties.layer = uiLayerID::Background;
                                    } else if(layerID.compare("sprite") == 0) {
                                        decorationPtr->uiProperties.layer = uiLayerID::Sprite;
                                    } else if(layerID.compare("text") == 0) {
                                        decorationPtr->uiProperties.layer = uiLayerID::Text;
                                    } else if(layerID.compare("window_frame") == 0) {
                                        decorationPtr->uiProperties.layer = uiLayerID::WindowFrame;
                                    }
                                } else if(propertyKeyIndex == (int)SceneKeyID::FontID) {
                                    decorationPtr->decProperties.fontID = findPropertyKey->get<std::string>();
                                } else if(propertyKeyIndex == (int)SceneKeyID::FontPath) {
                                    decorationPtr->decProperties.fontPath = findPropertyKey->get<std::string>();
                                } else if(propertyKeyIndex == (int)SceneKeyID::FontSize) {
                                    decorationPtr->decProperties.fontSize = findPropertyKey->get<int>();
                                } else if(propertyKeyIndex == (int)SceneKeyID::FontColor) {
                                    auto findFontColor = *findPropertyKey;
                                    if(findFontColor.is_string()) {
                                        std::string colorString(findFontColor.get<std::string>());
                                        if(colorString.compare("Black") == 0) {
                                            decorationPtr->decProperties.fontColor = sf::Color::Black;
                                        } else if(colorString.compare("White") == 0) {
                                            decorationPtr->decProperties.fontColor = sf::Color::White;
                                        } else if(colorString.compare("Red") == 0) {
                                            decorationPtr->decProperties.fontColor = sf::Color::Red;
                                        } else if(colorString.compare("Blue") == 0) {
                                            decorationPtr->decProperties.fontColor = sf::Color::Blue;
                                        } else if(colorString.compare("Green") == 0) {
                                            decorationPtr->decProperties.fontColor = sf::Color::Green;
                                        }
                                    } else {
                                        auto findR = findFontColor.find("r");
                                        auto findG = findFontColor.find("g");
                                        auto findB = findFontColor.find("b");
                                        auto findA = findFontColor.find("a");
                                        decorationPtr->decProperties.fontColor = {
                                                (sf::Uint8) findR->get<int>(),
                                                (sf::Uint8) findG->get<int>(),
                                                (sf::Uint8) findB->get<int>(),
                                                (sf::Uint8) findA->get<int>()
                                        };
                                    }
                                } else if(propertyKeyIndex == (int)SceneKeyID::OutlineColor) {
                                    auto findOutlineColor = *findPropertyKey;
                                    if(findOutlineColor.is_string()) {
                                        std::string colorString(findOutlineColor.get<std::string>());
                                        if(colorString.compare("Black") == 0) {
                                            decorationPtr->decProperties.outlineColor = sf::Color::Black;
                                        } else if(colorString.compare("White") == 0) {
                                            decorationPtr->decProperties.outlineColor = sf::Color::White;
                                        } else if(colorString.compare("Red") == 0) {
                                            decorationPtr->decProperties.outlineColor = sf::Color::Red;
                                        } else if(colorString.compare("Blue") == 0) {
                                            decorationPtr->decProperties.outlineColor = sf::Color::Blue;
                                        } else if(colorString.compare("Green") == 0) {
                                            decorationPtr->decProperties.outlineColor = sf::Color::Green;
                                        }
                                    } else {
                                        auto findR = findOutlineColor.find("r");
                                        auto findG = findOutlineColor.find("g");
                                        auto findB = findOutlineColor.find("b");
                                        auto findA = findOutlineColor.find("a");
                                        decorationPtr->decProperties.outlineColor = {
                                                (sf::Uint8) findR->get<int>(),
                                                (sf::Uint8) findG->get<int>(),
                                                (sf::Uint8) findB->get<int>(),
                                                (sf::Uint8) findA->get<int>()
                                        };
                                    }
                                } else if(propertyKeyIndex == (int)SceneKeyID::OutlineThickness) {
                                    decorationPtr->decProperties.outlineThickness = findPropertyKey->get<float>();
                                } else if(propertyKeyIndex == (int)SceneKeyID::Contents) {
                                    decorationPtr->decProperties.contents = findPropertyKey->get<std::string>();
                                } else if(propertyKeyIndex == (int)SceneKeyID::UIAction) {
                                    auto findUIAction = *findPropertyKey;
                                    auto findActionType = findUIAction.find(SceneKeys.at(SceneKeyID::UIActionType));
                                    auto findActionTrigger = findUIAction.find(SceneKeys.at(SceneKeyID::UIActionTrigger));
                                    auto findResourcePath = findUIAction.find(SceneKeys.at(SceneKeyID::ResourcePath));
                                    std::string typeString(findActionType->get<std::string>());
                                    std::string triggerString(findActionTrigger->get<std::string>());
                                    if(typeString.compare("transition_to_scene") == 0) {
                                        uiObjectPtr->action.type = uiActionType::TransitionToScene;
                                    } else {
                                        uiObjectPtr->action.type = uiActionType::None;
                                    }
                                    if(triggerString.compare("on_key_press") == 0) {
                                        uiObjectPtr->action.trigger = uiActionTrigger::OnKeyPress;
                                    } else {
                                        uiObjectPtr->action.trigger = uiActionTrigger::None;
                                    }
                                    uiObjectPtr->action.resourcePath = findResourcePath->get<std::string>();
                                } // if(key == UIAction)
                            } // if(findPropertyKey)
                        } // for(propertyKeyIndex)

                        if(decorationPtr) {
                            DecorationType decType(decorationPtr->decProperties.decType);
                            uiObjectProperties& uiProperties(decorationPtr->uiProperties);
                            DecorationProperties& decProperties(decorationPtr->decProperties);
                            ResourceSystem* resourceSystem(game->GetResourceSystem());
                            switch (decType) {
                                case DecorationType::Background:
                                    break;
                                case DecorationType::Doodad:
                                    break;
                                case DecorationType::Frame: {
                                    int textureWidth = uiProperties.textureSource.width;
                                    int textureHeight = uiProperties.textureSource.height;
                                    resourceSystem->LoadFrameTextures(decorationPtr->id,
                                                                      uiProperties.textureSource.pathToFile,
                                                                      uiProperties.textureSource.topLeft,
                                                                      textureWidth,
                                                                      textureHeight);
                                    for (int segmentIndex = 0;
                                         segmentIndex < (int) FrameSegmentID::TotalNumFrameSegments; ++segmentIndex) {
                                        UniqueID textureID = decorationPtr->id + FrameSegmentNames[segmentIndex];
                                        decorationPtr->texture = resourceSystem->GetTexture(textureID);
                                        AddFrameSegment(game, decorationPtr, (FrameSegmentID) segmentIndex);
                                    }
                                    break;
                                }
                                case DecorationType::Text: {
                                    decorationPtr->text = std::make_unique<sf::Text>();
                                    sf::Text* text = decorationPtr->text.get();
                                    sf::Font* font = resourceSystem->GetFont(decorationPtr->decProperties.fontID);
                                    if(!font) {
                                        resourceSystem->LoadResource(decorationPtr->decProperties.fontID, ResourceType::Font, decorationPtr->decProperties.fontPath);
                                        font = resourceSystem->GetFont(decorationPtr->decProperties.fontID);
                                    }
                                    decorationPtr->font = font;
                                    text->setFont(*font);
                                    text->setCharacterSize(decProperties.fontSize);
                                    text->setFillColor(decProperties.fontColor);
                                    text->setOutlineColor(decProperties.outlineColor);
                                    text->setOutlineThickness(decProperties.outlineThickness);
                                    text->setString(decProperties.contents);

                                    Position position(decorationPtr->uiProperties.position);
                                    if (uiProperties.align == Alignment::Center) {
                                        int offset = text->getGlobalBounds().width / 2;
                                        if (offset <= position.x) {
                                            position.x -= offset;
                                        } else {
                                            position.x = 0;
                                        }
                                    } else if (uiProperties.align == Alignment::Right) {
                                        int offset = text->getGlobalBounds().width;
                                        if (offset <= position.x) {
                                            position.x -= offset;
                                        } else {
                                            position.x = 0;
                                        }
                                    }
                                    text->setPosition(position.x, position.y);
                                    break;
                                }
                            }
                        }
                        AddToScene(scene, uiObjectPtr);
                    } // for(uiObj: uiObjectsJSON)

                } // if(keyID == uiObjects)
            } // if key found
        } // for each SceneKey
    } // for each property in Scene
    jsonFile.close();
    return scene;
}

void AddToScene(Scene* scene, uiObject* uiObjectPtr) {
    auto& drawLayer(scene->drawLayers[(int)uiObjectPtr->uiProperties.layer]);
    if(uiObjectPtr->sprite != nullptr) {
        drawLayer.push_back(uiObjectPtr->sprite.get());
    }
    if(uiObjectPtr->text != nullptr) {
        drawLayer.push_back(uiObjectPtr->text.get());
    }
    scene->uiObjects.push_back(std::unique_ptr<uiObject>(std::move(uiObjectPtr)));

    uiObject* sceneUI(scene->uiObjects.back().get());
    if(!sceneUI->children.empty()) {
        for(auto childIter = sceneUI->children.begin(); childIter != sceneUI->children.end(); ++childIter) {
            if((*childIter)->sprite != nullptr) {
                drawLayer.push_back((*childIter)->sprite.get());
            }
            if((*childIter)->text != nullptr) {
                drawLayer.push_back((*childIter)->text.get());
            }
        }
    }
}

void AddFrameSegment(Game* game, Decoration* frame, FrameSegmentID segmentID) {
    Position position{0,0};
    Position origin(frame->uiProperties.origin);
    const DisplayConfig& displayConfig(game->GetDisplayConfig());
    sf::Vector2u windowSize{ (unsigned int)displayConfig.windowProperties.width,
                             (unsigned int)displayConfig.windowProperties.height - displayConfig.windowHeightModifier };
    sf::Vector2u textureSize{ (unsigned int)(frame->uiProperties.textureSource.width * displayConfig.uiScaleX),
                              (unsigned int)(frame->uiProperties.textureSource.height * displayConfig.uiScaleY) };
    DisplaySystem* displaySystem(game->GetDisplaySystem());
    sf::Sprite* sprite(nullptr);
    Decoration* segment(nullptr);
    UniqueID childID(frame->id + FrameSegmentNames.at((int)segmentID));
    if(segmentID == FrameSegmentID::TopLeft ||
       segmentID == FrameSegmentID::TopRight ||
       segmentID == FrameSegmentID::BottomLeft ||
       segmentID == FrameSegmentID::BottomRight) {
        if(segmentID == FrameSegmentID::TopLeft) {
            position = origin;
        } // TopLeft
        else if(segmentID == FrameSegmentID::TopRight) {
            position = { (int)windowSize.x - (int)textureSize.x, origin.y };
        } // TopRight
        else if(segmentID == FrameSegmentID::BottomLeft) {
            position = { origin.x, (int)windowSize.y - (int)textureSize.y };
        } // BottomLeft
        else if(segmentID == FrameSegmentID::BottomRight) {
            position = { (int)windowSize.x - (int)textureSize.x,
                         (int)windowSize.y - (int)textureSize.y };
        } // BottomRight
        sprite = new sf::Sprite;
        sprite->setTexture(*frame->texture);
        game->ApplyUIScaling(sprite);
        sprite->setPosition(position.x, position.y);
        segment = game->CreateDecoration(childID, DecorationType::Frame );
        segment->sprite.reset(std::move(sprite));
        frame->children.push_back(std::unique_ptr<Decoration>(std::move(segment)));
    } // Corners only need 1 segment
    else if(segmentID == FrameSegmentID::TopMid) {
        position = { origin.x + (int)textureSize.x, origin.y };
        int segmentCount = ((int)windowSize.x / (int)textureSize.x) - 2;
        for(int segmentIndex = 0; segmentIndex < segmentCount; ++segmentIndex) {
            sprite = new sf::Sprite;
            sprite->setTexture(*frame->texture);
            game->ApplyUIScaling(sprite);
            sprite->setPosition(position.x, position.y);
            segment = game->CreateDecoration(childID, DecorationType::Frame);
            segment->sprite.reset(std::move(sprite));
            frame->children.push_back(std::unique_ptr<Decoration>(std::move(segment)));
            position.x += textureSize.x;
        }
    } // TopMid
    else if(segmentID == FrameSegmentID::BottomMid) {
        position = { origin.x + (int)textureSize.x, (int)windowSize.y - (int)textureSize.y };
        int segmentCount = ((int)windowSize.x / (int)textureSize.x) - 2;
        for(int segmentIndex = 0; segmentIndex < segmentCount; ++segmentIndex) {
            sprite = new sf::Sprite;
            sprite->setTexture(*frame->texture);
            game->ApplyUIScaling(sprite);
            sprite->setPosition(position.x, position.y);
            segment = game->CreateDecoration(childID, DecorationType::Frame);
            segment->sprite.reset(std::move(sprite));
            frame->children.push_back(std::unique_ptr<Decoration>(std::move(segment)));
            position.x += textureSize.x;
        }
    } // BottomMid
    else if(segmentID == FrameSegmentID::MidLeft) {
        position = { origin.x, origin.y + (int)textureSize.y };
        int segmentCount = ((int)windowSize.y / (int)textureSize.y) - 2;
        for(int segmentIndex = 0; segmentIndex < segmentCount; ++segmentIndex) {
            sprite = new sf::Sprite;
            sprite->setTexture(*frame->texture);
            game->ApplyUIScaling(sprite);
            sprite->setPosition(position.x, position.y);
            segment = game->CreateDecoration(childID, DecorationType::Frame);
            segment->sprite.reset(std::move(sprite));
            frame->children.push_back(std::unique_ptr<Decoration>(std::move(segment)));
            position.y += textureSize.y;
        }
    } // MidLeft
    else if(segmentID == FrameSegmentID::MidRight) {
        position = { (int)windowSize.x - (int)textureSize.x, origin.y + (int)textureSize.y };
        int segmentCount = ((int)windowSize.y / (int)textureSize.y) - 2;
        for(int segmentIndex = 0; segmentIndex < segmentCount; ++segmentIndex) {
            sprite = new sf::Sprite;
            sprite->setTexture(*frame->texture);
            game->ApplyUIScaling(sprite);
            sprite->setPosition(position.x, position.y);
            segment = game->CreateDecoration(childID, DecorationType::Frame);
            segment->sprite.reset(std::move(sprite));
            frame->children.push_back(std::unique_ptr<Decoration>(std::move(segment)));
            position.y += textureSize.y;
        }
    } // MidRight
    else if(segmentID == FrameSegmentID::Middle) {
        position = { origin.x + (int)textureSize.x, origin.y + (int)textureSize.y };
        int rowCount = (((int)windowSize.y) / (int)textureSize.y) - 2;
        int columnCount = ((int)windowSize.x / (int)textureSize.x) - 2;
        for(int y = 0; y < rowCount; ++y) {
            for(int x = 0; x < columnCount; ++x) {
                sprite = new sf::Sprite;
                sprite->setTexture(*frame->texture);
                game->ApplyUIScaling(sprite);
                sprite->setPosition(position.x, position.y);
                segment = game->CreateDecoration(childID, DecorationType::Frame);
                segment->sprite.reset(std::move(sprite));
                frame->children.push_back(std::unique_ptr<Decoration>(std::move(segment)));
                position.x += textureSize.x;
            }
            position.x = origin.x + (int)textureSize.x;
            position.y += textureSize.y;
        }
    } // Middle
}

bool ContainsOperator(std::string_view arithmeticString) {
    bool operatorFound(false);
    std::string calcString(arithmeticString);
    std::size_t addPosition(calcString.find('+'));
    std::size_t subtractPosition(calcString.find('-'));
    std::size_t multiplyPosition(calcString.find('*'));
    std::size_t dividePosition(calcString.find('/'));
    if(addPosition != std::string::npos ||
        subtractPosition != std::string::npos ||
        multiplyPosition != std::string::npos ||
        dividePosition != std::string::npos) {
        operatorFound = true;
    }
    return operatorFound;
}

GameplayTransition::GameplayTransition(Game* gamePtr): InputListener(gamePtr), game(gamePtr) {

}

void GameplayTransition::ReceiveInput(const sf::Event& event) {
    if(event.type == sf::Event::EventType::KeyPressed) {
        std::string scenePath(game->GetResourceSystem()->GetScenePath(SceneNames.at(SceneID::Gameplay)));
        Scene* gameplay = LoadScene(scenePath, game);
        game->TransitionTo(gameplay);
    }
}

