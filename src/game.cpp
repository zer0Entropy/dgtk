//
// Created by zeroc00l on 10/31/23.
//

#include "../include/game.hpp"
#include "../include/log.hpp"
#include "../include/input.hpp"
#include "../include/resource.hpp"
#include "../include/decoration.hpp"
#include "../include/view.hpp"

const std::string Game::configFilename = "game.cfg";

Game::Game(const std::filesystem::path& workingDir, unsigned int seed):
    rng(seed),
    workingDirectory(workingDir),
    status(GameStatus::MainMenu),
    currentScene(nullptr) {

}

Game::~Game() {

}

void Game::Init() {
    bool configFound(FindGameConfig());
    if(!configFound) {
        status = GameStatus::Error;
        std::string errorType("FileNotFound");
        std::string errorDescription("The following file could not be found: " + configFilename);
        systems[(int)SystemID::Log] = std::make_unique<LogSystem>(workingDirectory);
        LogSystem* logSystem = static_cast<LogSystem*>(systems[(int)SystemID::Log].get());
        logSystem->RecordError(errorType, errorDescription);
        return;
    }
    LoadGameConfig();
    for(int systemIndex = 0; systemIndex < (int)SystemID::TotalNumSystems; ++systemIndex) {
        switch((SystemID)systemIndex) {
            case SystemID::Display:
                systems[systemIndex] = std::make_unique<DisplaySystem>(displayConfig, this);
                break;
            case SystemID::Input:
                systems[systemIndex] = std::make_unique<InputSystem>(static_cast<DisplaySystem*>(systems[(int)SystemID::Display].get()));
                break;
            case SystemID::Log:
                systems[systemIndex] = std::make_unique<LogSystem>(projectRoot);
                break;
            case SystemID::Resource:
                systems[systemIndex] = std::make_unique<ResourceSystem>(resourceDirectory);
                break;
        }
    } // for(systemIndex)
    for(int systemIndex = 0; systemIndex < (int)SystemID::TotalNumSystems; ++systemIndex) {
        systems[systemIndex]->Init();
    }
    mathParser.RegisterVariable("window_width", displayConfig.windowProperties.width);
    mathParser.RegisterVariable("window_height", displayConfig.windowProperties.height);
    mathParser.RegisterVariable("frame_modifier", displayConfig.windowHeightModifier);
}

void Game::Update() {
    for(int systemIndex = 0; systemIndex < (int)SystemID::TotalNumSystems; ++systemIndex) {
        System* system = systems[systemIndex].get();
        if(system) {
            systems[systemIndex]->Update();
        }
    }
}

void Game::Shutdown() {
    SignalInterrupt();
    for(int systemIndex = 0; systemIndex < (int)SystemID::TotalNumSystems; ++systemIndex) {
        if(systems[systemIndex] != nullptr) {
            systems[systemIndex]->Shutdown();
        }
    }
    for(int systemIndex = 0; systemIndex < (int)SystemID::TotalNumSystems; ++systemIndex) {
        systems[systemIndex].reset(nullptr);
    }
}

GameStatus Game::GetCurrentStatus() const {
    return status;
}

DisplaySystem* Game::GetDisplaySystem() const {
    return static_cast<DisplaySystem*>(systems[(int)SystemID::Display].get());
}

InputSystem* Game::GetInputSystem() const {
    return static_cast<InputSystem*>(systems[(int)SystemID::Input].get());
}

LogSystem* Game::GetLogSystem() const {
    return static_cast<LogSystem*>(systems[(int)SystemID::Log].get());
}

ResourceSystem* Game::GetResourceSystem() const {
    return static_cast<ResourceSystem*>(systems[(int)SystemID::Resource].get());
}

const DisplayConfig& Game::GetDisplayConfig() const {
    return displayConfig;
}

MathParser& Game::GetMathParser() const {
    return (MathParser&)mathParser;
}

RandomNumberGenerator& Game::GetRNG() const {
    return (RandomNumberGenerator&)rng;
}

Scene* Game::GetCurrentScene() const {
    return currentScene.get();
}

void Game::ApplyUIScaling(sf::Transformable* transform) {
    transform->setScale(displayConfig.uiScaleX, displayConfig.uiScaleY);
}

void Game::ApplyTileScaling(sf::Transformable* transform) {
    transform->setScale(displayConfig.tileScaleX, displayConfig.tileScaleY);
}

void Game::TransitionTo(Scene* scene) {
    InputSystem* inputSystem(GetInputSystem());
    ResourceSystem* resourceSystem(GetResourceSystem());

    if(currentScene) {
        SignalInterrupt();

        for(auto iterator = currentScene->keyListeners.begin();
            iterator != currentScene->keyListeners.end(); ++iterator) {
            inputSystem->RemoveListener(iterator->second, ListenerType::KeyPressListener);
        } // for each inputListener in Scene

        Scene* oldScene = currentScene.release();
        delete oldScene;
        currentScene.reset(std::move(scene));
    }
    else {
        currentScene.reset(std::move(scene));
    }

    for(auto decorationIter = scene->properties.decorations.begin(); decorationIter != scene->properties.decorations.end(); ++decorationIter) {
        uiObjectProperties uiObjProperties = decorationIter->first;
        DecorationProperties decProperties = decorationIter->second;
        Decoration* decoration = CreateDecoration(uiObjProperties, decProperties);
        scene->uiObjects.push_back(decoration);
    }

    for(auto actionIter = scene->properties.actions.begin(); actionIter != scene->properties.actions.end(); ++actionIter) {
        Action action = actionIter->second;
        ActionType actionType = action.type;
        ActionTrigger actionTrigger = action.trigger;
        KeyPressListener* keyListener(nullptr);
        if(actionTrigger == ActionTrigger::OnKeyPress && actionType != ActionType::None) {
            auto listenIter = scene->keyListeners.find(action.actorID);
            if(listenIter != scene->keyListeners.end()) {
                 keyListener = static_cast<KeyPressListener*>(listenIter->second);
            } else {
                keyListener = new KeyPressListener(this);
                scene->keyListeners.insert(std::make_pair(action.actorID, keyListener));
            }
            keyListener->AddKeyMapping(action.triggerKey, action);
        }
    }

    for(auto iterator = currentScene->keyListeners.begin(); iterator != currentScene->keyListeners.end(); ++iterator) {
        inputSystem->AddListener(iterator->second, ListenerType::KeyPressListener);
    } // for each inputListener in Scene

    if(!scene->properties.mapProperties.name.empty()) {
        scene->map = std::unique_ptr<Map>(new Map);
        Map& map = *scene->map.get();
        map.properties = scene->properties.mapProperties;
        for(int y = 0; y < MaxMapHeight; ++y) {
            for(int x = 0; x < MaxMapWidth; ++x) {
                Tile& tile = map.tileArray[y][x];
                tile.terrain = nullptr;
                tile.creature = nullptr;
                tile.sprite.reset(nullptr);
                tile.isVisible = false;
            }
        }
        for(int terrainIndex = 1; terrainIndex < (int)TerrainType::TotalNumTerrainTypes; ++terrainIndex) {
            TerrainType type = (TerrainType)terrainIndex;
            TerrainProperties& terrain = scene->map->properties.terrainProperties.at(type);
            resourceSystem->LoadTexture(    TerrainTypeNames.at(terrainIndex),
                                            scene->map->properties.texturePath,
                                            terrain.texturePosition,
                                            scene->map->properties.textureWidth,
                                            scene->map->properties.textureHeight);
            terrain.texture = resourceSystem->GetTexture(TerrainTypeNames.at(terrainIndex));
        }

        sf::IntRect area = {
                0,
                0,
                scene->map->properties.width,
                scene->map->properties.height
        };
        int minWidth(4);
        int minHeight(5);
        float minRatio(0.5);
        float maxRatio(0.55);
        int maxWidth(15);
        int maxHeight(12);
        BSP::Tree bspTree(rng);
        bspTree.CreateRootNode(area);
        int tallestRoom(0);
        int widestRoom(0);
        do {
            bspTree.Split(minRatio, maxRatio, minWidth, minHeight);
            bspTree.GetLeafList();
            widestRoom = bspTree.GetMaxNodeWidth();
            tallestRoom = bspTree.GetMaxNodeHeight();
        } while(widestRoom > maxWidth || tallestRoom > maxHeight);

        auto areaList = bspTree.GetLeafValues();

        int roomIndex(0);
        for(auto possibleRoom : areaList) {
            if(possibleRoom.width > 4 && possibleRoom.height > 4) {
                UniqueID roomID("Room#");
                roomID.append(std::to_string(roomIndex++));
                scene->map->properties.roomList.insert(std::make_pair(roomID,
                                                                        Room(MapLocation{possibleRoom.left, possibleRoom.top},
                                                                         possibleRoom.width,
                                                                         possibleRoom.height,
                                                                         roomID,
                                                                         MapLocation{possibleRoom.left + (possibleRoom.width / 2), possibleRoom.top + (possibleRoom.height / 2)})) );
            }
        }

        GenerateMap(*scene->map.get(), rng, displayConfig);

        Room* randomRoom(nullptr);
        roomIndex = rng.GetRandom(0, map.properties.roomList.size());
        int count(0);
        for(auto roomIter = map.properties.roomList.begin(); roomIter != map.properties.roomList.end(); ++roomIter, ++count) {
            randomRoom = &roomIter->second;
            if(count == roomIndex) {
                break;
            }
        }

        UniqueID playerName("Player1");
        sf::Texture* playerTexture(nullptr);
        std::string texturePath("/texture/oryx/oryx_16bit_fantasy_creatures_trans.png");
        Position texturePosition = { 24, 24 };
        int textureWidth(24), textureHeight(24);
        MapLocation playerLocation = {randomRoom->center.x,randomRoom->center.y};
        resourceSystem->LoadTexture(playerName, texturePath, texturePosition, textureWidth, textureHeight);
        playerTexture = resourceSystem->GetTexture(playerName);
        Player* player = CreatePlayer(playerName, playerTexture, playerLocation, map.properties);
        player->currentArea = randomRoom;
        (map.tileArray[playerLocation.y][playerLocation.x]).creature = player->character.get();
        scene->creatures.push_back(player->character.get());
        scene->player.reset(std::move(player));

        scene->mapView = new MapView;
        scene->mapView->properties = scene->properties.viewProperties;
        scene->mapView->properties.widthInTiles = scene->mapView->properties.widthInPixels / (textureWidth * displayConfig.tileScaleX);
        scene->mapView->properties.heightInTiles = scene->mapView->properties.heightInPixels / (textureHeight * displayConfig.tileScaleY);
        CenterViewOnPlayer(*scene->mapView, *scene->map.get(), playerLocation);

        InitVisibilityMap(map, scene->mapView->visibilityMap);
    }
}

Creature* Game::FindCreature(UniqueID id) {
    Creature* creature(nullptr);
    for(auto creatureIter = currentScene->creatures.begin(); !creature && creatureIter != currentScene->creatures.end(); ++creatureIter) {
        if((*creatureIter)->properties.name == id) {
            creature = (*creatureIter);
        }
    }
    return creature;
}

bool Game::MoveCreature(Creature* creature, MapLocation location) {
    bool success(false);
    MapLocation oldLocation(creature->properties.location);
    Map& map(*currentScene->map);
    Tile& oldTile(map.tileArray[oldLocation.y][oldLocation.x]);
    int mapWidth(map.properties.width);
    int mapHeight(map.properties.height);
    if(location.x >= 0 && location.x < mapWidth
        && location.y >= 0 && location.y < mapHeight) {
        Tile& newTile(map.tileArray[location.y][location.x]);
        if(newTile.terrain && newTile.terrain->isWalkable && !newTile.creature) {
            oldTile.creature = nullptr;
            newTile.creature = creature;
            creature->properties.location = location;
            success = true;
        }
    } // check if location is valid
    if(success && creature->properties.name.compare(currentScene->player->character->properties.name) == 0) {
        CenterViewOnPlayer(*currentScene->mapView, *currentScene->map.get(), creature->properties.location);
        UpdateVisibilityMap(map, currentScene->mapView->visibilityMap, creature->properties.location);
        currentScene->player->currentArea = FindArea(map, creature->properties.location);
    }
    return success;
}

bool Game::FindGameConfig() {
    bool success(false);
    std::filesystem::path currentDirectory(workingDirectory);
    std::string configPath(currentDirectory);
    while(currentDirectory != currentDirectory.root_directory()) {
        configPath = currentDirectory;
        configPath.append("/" + configFilename);
        if(std::filesystem::exists(configPath)) {
            success = true;
            configFilePath = configPath;
            projectRoot = configFilePath.parent_path();
            std::string resourcePath(projectRoot);
            resourcePath.append("/resource");
            resourceDirectory = resourcePath;
            std::string configPath(resourcePath);
            configPath.append("/config");
            configDirectory = configPath;
            break;
        }
        else {
            currentDirectory = currentDirectory.parent_path();
        }
    }
    return success;
}

bool Game::LoadGameConfig() {
    bool success(false);
    std::ifstream configFile(configFilePath, std::ios_base::in);
    if(configFile.is_open()) {
        success = true;
    }
    int maxLineLength(255);
    while(configFile.good()) {
        char cString[maxLineLength];
        configFile.getline(cString, maxLineLength);
        std::string configLine(cString);
        std::size_t pos(configLine.find('='));
        if(configLine.find("ui_scale_x") != std::string::npos) {
            while(!std::isdigit(configLine.at(pos))) {
                pos++;
            }
            std::string xScaleString(configLine.substr(pos, configLine.length() - pos));
            displayConfig.uiScaleX = atof(xScaleString.c_str());
        }
        else if(configLine.find("ui_scale_y") != std::string::npos) {
            while(!std::isdigit(configLine.at(pos))) {
                pos++;
            }
            std::string yScaleString(configLine.substr(pos, configLine.length() - pos));
            displayConfig.uiScaleY = atof(yScaleString.c_str());
        }
        else if(configLine.find("tile_scale_x") != std::string::npos) {
            while(!std::isdigit(configLine.at(pos))) {
                pos++;
            }
            std::string xScaleString(configLine.substr(pos, configLine.length() - pos));
            displayConfig.tileScaleX = atof(xScaleString.c_str());
        }
        else if(configLine.find("tile_scale_y") != std::string::npos) {
            while(!std::isdigit(configLine.at(pos))) {
                pos++;
            }
            std::string yScaleString(configLine.substr(pos, configLine.length() - pos));
            displayConfig.tileScaleY = atof(yScaleString.c_str());
        }
        else if(configLine.find("window_width") != std::string::npos) {
            while(!std::isdigit(configLine.at(pos))) {
                pos++;
            }
            std::string widthString(configLine.substr(pos, configLine.length() - pos));
            displayConfig.windowProperties.width = atoi(widthString.c_str());
        }
        else if(configLine.find("window_height") != std::string::npos) {
            while(!std::isdigit(configLine.at(pos))) {
                pos++;
            }
            std::string heightString(configLine.substr(pos, configLine.length() - pos));
            displayConfig.windowProperties.height = atoi(heightString.c_str());
        }
        else if(configLine.find("window_title") != std::string::npos) {
            while(!std::isalpha(configLine.at(pos))) {
                pos++;
            }
            std::string titleString(configLine.substr(pos, configLine.length() - pos - 1));
            displayConfig.windowProperties.title = titleString;
        }
        else if(configLine.find("win_height_modifier") != std::string::npos) {
            while(!std::isdigit(configLine.at(pos))) {
                pos++;
            }
            std::string heightModString(configLine.substr(pos, configLine.length() - pos));
            displayConfig.windowHeightModifier = atoi(heightModString.c_str());
        }
    } // while(good())
    if(configFile.is_open()) {
        configFile.close();
    }
    return success;
}

Decoration* Game::CreateFrameSegment(Decoration* frame, FrameSegmentID segmentID) {
    Decoration* frameSegment(nullptr);

    Position position(frame->uiProperties.position);
    Position origin(frame->uiProperties.origin);
    UniqueID frameSegmentID(frame->id);
    frameSegmentID.append(FrameSegmentNames.at((int)segmentID));

    const TextureSource& source(frame->uiProperties.textureSource);
    const Position textureOrigin(source.topLeft);
    Position texturePosition(source.topLeft);
    sf::Sprite* sprite(nullptr);

    sf::Vector2u windowSize{ (unsigned int)displayConfig.windowProperties.width,
                             (unsigned int)displayConfig.windowProperties.height }; //- displayConfig.windowHeightModifier };
    sf::Vector2u textureSize{ (unsigned int)(source.width * displayConfig.uiScaleX),
                              (unsigned int)(source.height * displayConfig.uiScaleY) };

    switch(segmentID) {
        case FrameSegmentID::TopLeft:
            position = origin;
            texturePosition = textureOrigin;
            break;
        case FrameSegmentID::TopMid:
            position = { origin.x + (int)textureSize.x, origin.y };
            texturePosition = { textureOrigin.x + source.width, textureOrigin.y };
            break;
        case FrameSegmentID::TopRight:
            position = { (int)windowSize.x - (int)textureSize.x, origin.y };
            texturePosition = {textureOrigin.x + (2 * source.width), textureOrigin.y };
            break;
        case FrameSegmentID::MidLeft:
            position = { origin.x, origin.y + (int)textureSize.y };
            texturePosition = { textureOrigin.x, textureOrigin.y + source.height };
            break;
        case FrameSegmentID::Middle:
            position = { origin.x + (int)textureSize.x, origin.y + (int)textureSize.y };
            texturePosition = { textureOrigin.x + source.width, textureOrigin.y + source.height };
            break;
        case FrameSegmentID::MidRight:
            position = { (int)windowSize.x - (int)textureSize.x, origin.y + (int)textureSize.y };
            texturePosition = { textureOrigin.x + (2 * source.width), textureOrigin.y + source.height };
            break;
        case FrameSegmentID::BottomLeft:
            position = { origin.x, (int)windowSize.y - (int)textureSize.y };
            texturePosition = { textureOrigin.x, textureOrigin.y + (2 * source.height) };
            break;
        case FrameSegmentID::BottomMid:
            position = { origin.x + (int)textureSize.x, (int)windowSize.y - (int)textureSize.y };
            texturePosition = { textureOrigin.x + source.width, textureOrigin.y + (2 * source.height) };
            break;
        case FrameSegmentID::BottomRight:
            position = { (int)windowSize.x - (int)textureSize.x, (int)windowSize.y - (int)textureSize.y };
            texturePosition = { textureOrigin.x + (2 * source.width), textureOrigin.y + (2 * source.height) };
            break;
        case FrameSegmentID::TotalNumFrameSegmentIDs:   default:
            break;
    }

    ResourceSystem* resourceSystem = GetResourceSystem();
    sf::Texture* texture = resourceSystem->GetTexture(frameSegmentID);
    if(!texture) {
        resourceSystem->LoadTexture(frameSegmentID, source.pathToFile, texturePosition, source.width, source.height);
        texture = resourceSystem->GetTexture(frameSegmentID);
    }

    frameSegment = new Decoration(frameSegmentID, DecorationType::Frame);
    frameSegment->uiProperties = frame->uiProperties;
    frameSegment->decProperties = frame->decProperties;
    frameSegment->uiProperties.id = frameSegment->decProperties.id = frameSegmentID;
    frameSegment->texture = texture;
    frameSegment->uiProperties.position = position;

    sprite = new sf::Sprite;
    sprite->setTexture(*texture);
    ApplyUIScaling(sprite);
    sprite->setPosition(position.x, position.y);
    frameSegment->sprite.reset(std::move(sprite));

    return frameSegment;
}

Decoration* Game::CreateFrame(const uiObjectProperties& uiObjProperties, const DecorationProperties& decProperties) {
    Decoration* frame(new Decoration(decProperties.id, decProperties.decType));
    frame->uiProperties = uiObjProperties;
    frame->decProperties = decProperties;
    int segmentCount(0), rowCount(0), columnCount(0);

    sf::Vector2i textureSize{ (int)(uiObjProperties.textureSource.width * displayConfig.uiScaleX),
                              (int)(uiObjProperties.textureSource.height * displayConfig.uiScaleY) };

    Position position(uiObjProperties.position);
    const Position origin(position);
    int width(decProperties.frameWidth);
    int height(decProperties.frameHeight);

    for(int segmentIndex = 0; segmentIndex < (int)FrameSegmentID::TotalNumFrameSegmentIDs; ++segmentIndex) {
        std::string segmentID(decProperties.id);
        int stepX(0), stepY(0);
        segmentID.append(FrameSegmentNames.at(segmentIndex));

        if ((FrameSegmentID) segmentIndex == FrameSegmentID::TopLeft) {
            position = origin;
            segmentCount = 1;
        } else if((FrameSegmentID) segmentIndex == FrameSegmentID::TopRight) {
            position = {
                    origin.x + width - textureSize.x,
                        origin.y
            };
            segmentCount = 1;
        } else if((FrameSegmentID) segmentIndex == FrameSegmentID::BottomLeft) {
            position = {
                    origin.x,
                    origin.y + height - textureSize.y
            };
            segmentCount = 1;
        } else if((FrameSegmentID) segmentIndex == FrameSegmentID::BottomRight) {
            position = {
                    origin.x + width - textureSize.x,
                    origin.y + height - textureSize.y
            };
            segmentCount = 1;
        } else if ((FrameSegmentID) segmentIndex == FrameSegmentID::TopMid) {
            position = {
                    origin.x + textureSize.x,
                    origin.y
            };
            segmentCount = (width - (2 * textureSize.x)) / textureSize.x;
            stepX = 1;
        } else if((FrameSegmentID) segmentIndex == FrameSegmentID::BottomMid) {
            position = {
                    origin.x + textureSize.x,
                    origin.y + height - textureSize.y
            };
            segmentCount = (width - (2 * textureSize.x)) / textureSize.x;
            stepX = 1;
        } else if ((FrameSegmentID) segmentIndex == FrameSegmentID::MidLeft) {
            position = {
                    origin.x,
                    origin.y + textureSize.y
            };
            segmentCount = (height - (2 * textureSize.y)) / textureSize.y;
            stepY = 1;
        } else if((FrameSegmentID) segmentIndex == FrameSegmentID::MidRight) {
            position = {
                    origin.x + width - textureSize.x,
                    origin.y + textureSize.y
            };
            segmentCount = (height - (2 * textureSize.y)) / textureSize.y;
            stepY = 1;
        } else if((FrameSegmentID) segmentIndex == FrameSegmentID::Middle) {
            position = {
                    origin.x + textureSize.x,
                    origin.y + textureSize.y
            };
            rowCount = (height - (2 * textureSize.y)) / textureSize.y;
            columnCount = (width - (2 * textureSize.x)) / textureSize.x;
            segmentCount = rowCount * columnCount;
            stepX = 1;
            stepY = 1;
        }
        for (int s = 0; s < segmentCount; ++s) {
            Decoration* segment = CreateFrameSegment(frame, (FrameSegmentID) segmentIndex);
            segment->sprite->setPosition(position.x, position.y);
            position.x += (stepX * textureSize.x);
            if((FrameSegmentID)segmentIndex == FrameSegmentID::Middle) {
                if(position.x - origin.x + textureSize.x >= width) {
                    position.x = origin.x + textureSize.x;
                    position.y += (stepY * textureSize.y);
                }
            } else {
                position.y += (stepY * textureSize.y);
            }
            frame->children.push_back(std::unique_ptr<uiObject>(std::move(segment)));
        }
    }
    return frame;
}

Decoration* Game::CreateDecoration(const uiObjectProperties& uiObjProperties, const DecorationProperties& decProperties) {
    Decoration* decoration(nullptr);
    ResourceSystem* resourceSystem(GetResourceSystem());
    bool success(true);

    sf::Vector2i windowSize{ displayConfig.windowProperties.width,
                             displayConfig.windowProperties.height - displayConfig.windowHeightModifier };

    switch(decProperties.decType) {
        case DecorationType::Background:
            break;
        case DecorationType::Doodad:
            break;
        case DecorationType::Frame:
            decoration = CreateFrame(uiObjProperties, decProperties);
            return decoration;
            break;
        case DecorationType::Text:
            break;
        default:
            success = false;
    }

    if(success) {
        decoration = new Decoration(decProperties.id, decProperties.decType);
        decoration->font = nullptr;
        decoration->texture = nullptr;
        decoration->sprite = nullptr;
        decoration->text = nullptr;
        decoration->uiProperties = uiObjProperties;
        decoration->decProperties = decProperties;
        switch(decProperties.decType) {
            case DecorationType::Background:
                decoration->sprite = std::unique_ptr<sf::Sprite>(new sf::Sprite);
                break;
            case DecorationType::Doodad:
                decoration->sprite = std::unique_ptr<sf::Sprite>(new sf::Sprite);
                break;
            case DecorationType::Frame:
                break;
            case DecorationType::Text:
                decoration->text = std::unique_ptr<sf::Text>(new sf::Text);
                if(!decoration->font) {
                    resourceSystem->LoadResource(  decProperties.fontID,
                                                   ResourceType::Font,
                                                   decProperties.fontPath  );
                    decoration->font = resourceSystem->GetFont(decProperties.fontID);
                }
                decoration->text->setFont(*decoration->font);
                decoration->text->setCharacterSize(decProperties.fontSize);
                decoration->text->setFillColor(decProperties.fontColor);
                decoration->text->setOutlineColor(decProperties.outlineColor);
                decoration->text->setOutlineThickness(decProperties.outlineThickness);
                decoration->text->setString(decProperties.contents);
                decoration->text->setPosition(decoration->uiProperties.position.x, decoration->uiProperties.position.y);
                if(uiObjProperties.align == Alignment::Center) {
                    Position centeredPosition = {
                            decoration->uiProperties.position.x - (int)(decoration->text->getGlobalBounds().width / 2),
                            decoration->uiProperties.position.y
                    };
                    decoration->text->setPosition(centeredPosition.x, centeredPosition.y);
                } else if(uiObjProperties.align == Alignment::Right) {
                    Position position = decoration->uiProperties.position;
                    position.x += decoration->text->getLocalBounds().width;
                    if(position.x + decoration->text->getLocalBounds().width > windowSize.x) {
                        position.x = windowSize.x - decoration->text->getLocalBounds().width;
                    }
                    decoration->text->setPosition(decoration->uiProperties.position.x, decoration->uiProperties.position.y);
                }
                break;
            default:    break;
        }
    }

    if(decoration->sprite) {
        if(!decoration->texture) {
            resourceSystem->LoadTexture(    decoration->decProperties.id,
                                            decoration->uiProperties.textureSource.pathToFile,
                                            decoration->uiProperties.textureSource.topLeft,
                                            decoration->uiProperties.textureSource.width,
                                            decoration->uiProperties.textureSource.height   );
            decoration->texture = resourceSystem->GetTexture(decoration->decProperties.id);
            if(decoration->texture) {
                decoration->sprite->setTexture(*decoration->texture);
            } else {
                sf::Sprite* sprite = decoration->sprite.release();
                delete sprite;
            }
        }
    }

    return decoration;
}

void Game::SignalInterrupt() {
    for(auto& system : systems) {
        System* sysPtr = system.get();
        if(sysPtr) {
            sysPtr->SetInterrupt();
        }
    }
}

Player* Game::CreatePlayer(std::string name, sf::Texture* texture, MapLocation location, const MapProperties& mapProperties) {
    Player* player = new Player;
    player->character.reset(std::move(CreateCreature(name, texture, location, mapProperties)));
    return player;
}

Creature* Game::CreateCreature(std::string name, sf::Texture* texture, MapLocation location, const MapProperties& mapProperties) {
    Creature* creature = new Creature;
    creature->properties.name = name;
    creature->texture = texture;
    creature->properties.location = location;
    creature->position = { (int)(location.x * mapProperties.textureWidth * displayConfig.tileScaleX),
                           (int)(location.y * mapProperties.textureHeight * displayConfig.tileScaleY) };
    creature->sprite.reset(new sf::Sprite);
    creature->sprite->setTexture(*creature->texture);
    creature->sprite->setScale(displayConfig.tileScaleX, displayConfig.tileScaleY);
    creature->sprite->setPosition(creature->position.x, creature->position.y);
    return creature;
}

