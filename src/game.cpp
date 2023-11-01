//
// Created by zeroc00l on 10/31/23.
//

#include "../include/game.hpp"
#include "../include/log.hpp"
#include "../include/input.hpp"
#include "../include/resource.hpp"
#include "../include/decoration.hpp"

const std::string Game::configFilename = "game.cfg";

Game::Game(const std::filesystem::path& workingDir):
    workingDirectory(workingDir), status(GameStatus::MainMenu), currentScene(nullptr) {
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
}

void Game::Update() {
    for(int systemIndex = 0; systemIndex < (int)SystemID::TotalNumSystems; ++systemIndex) {
        systems[systemIndex]->Update();
    }
}

void Game::Shutdown() {
    for(int systemIndex = 0; systemIndex < (int)SystemID::TotalNumSystems; ++systemIndex) {
        systems[systemIndex]->Shutdown();
    }
}

GameStatus Game::GetCurrentStatus() const {
    return status;
}

Decoration* Game::CreateDecoration(UniqueID id, const uiObjectProperties& uiProperties, const DecorationProperties& properties) {
    Decoration* decoration(nullptr);
    ResourceSystem* resourceSystem(GetResourceSystem());
    DisplaySystem* displaySystem(GetDisplaySystem());
    sf::Font* font(resourceSystem->GetFont(properties.fontID));
    sf::Texture* texture(nullptr);
    sf::Sprite* sprite(nullptr);
    sf::Text* text(nullptr);
    std::string textureID;
    Position position{uiProperties.position};
    unsigned int textureWidth, textureHeight;
    switch(properties.decType) {
        case DecorationType::Background:
            // TODO!
            break;
        case DecorationType::Frame:
            textureWidth = uiProperties.textureSource.width / 3;
            textureHeight = uiProperties.textureSource.height / 3;
            resourceSystem->LoadFrameTextures(id,
                                              uiProperties.textureSource.pathToFile,
                                              uiProperties.textureSource.topLeft,
                                              textureWidth,
                                              textureHeight);
            decoration = new Decoration(id, DecorationType::Frame);
            decoration->uiProperties = uiProperties;
            decoration->decProperties = properties;
            for(int segmentIndex = 0; segmentIndex < (int)FrameSegment::TotalNumFrameSegments; ++segmentIndex) {
                textureID = id + FrameSegmentNames[segmentIndex];
                decoration->texture = resourceSystem->GetTexture(textureID);
                AddFrameSegment(decoration, (FrameSegment)segmentIndex);
            }
            break;
        case DecorationType::Doodad:
            // TODO!
            break;
        case DecorationType::Text:
                if(!font) {
                    resourceSystem->LoadResource(properties.fontID, ResourceType::Font, properties.fontPath);
                    font = resourceSystem->GetFont(properties.fontID);
                }
                decoration = new Decoration(id, DecorationType::Text);
                decoration->uiProperties = uiProperties;
                decoration->decProperties = properties;
                decoration->font = font;
                text = new sf::Text;
                currentScene->textList.push_back(text);
                text->setFont(*font);
                text->setCharacterSize(properties.fontSize);
                text->setFillColor(properties.fontColor);
                text->setOutlineColor(properties.outlineColor);
                text->setOutlineThickness(properties.outlineThickness);
                text->setString(properties.contents);
                if(uiProperties.align == Alignment::Center) {
                    int offset = text->getGlobalBounds().width / 2;
                    if(offset <= position.x) {
                        position.x -= offset;
                    } else {
                        position.x = 0;
                    }
                }
                else if(uiProperties.align == Alignment::Right) {
                    int offset = text->getGlobalBounds().width;
                    if(offset <= position.x) {
                        position.x -= offset;
                    } else {
                        position.x = 0;
                    }
                }
                text->setPosition(position.x, position.y);
            break;
    }
    return decoration;
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

Scene* Game::GetCurrentScene() const {
    return currentScene.get();
}

Scene* Game::GenerateScene(GameStatus nextStatus) {
    Scene* scene(new Scene);
    WindowProperties windowProperties(displayConfig.windowProperties);
    ResourceSystem* resourceSystem(GetResourceSystem());
    switch(nextStatus) {
        case GameStatus::Error:
            break;
        case GameStatus::MainMenu: {
            uiObjectProperties frameProperties;
            frameProperties.uiType = uiObjectType::Decoration;
            frameProperties.textureSource.pathToFile = "/texture/oryx/oryx_16bit_fantasy_world.png";
            frameProperties.textureSource.width = 72;
            frameProperties.textureSource.height = 72;
            frameProperties.textureSource.topLeft = { 192, 576 };
            frameProperties.origin = {0, 0};
            UniqueID frameID("WindowFrame");
            scene->uiProperties.insert(std::make_pair(frameID, frameProperties));
            DecorationProperties frameDecProperties;
            frameDecProperties.decType = DecorationType::Frame;
            scene->decorationProperties.insert(std::make_pair(frameID, frameDecProperties));

            int windowWidth(windowProperties.width);
            int windowHeight(windowProperties.height - displayConfig.windowHeightModifier);

            uiObjectProperties titleUI;
            titleUI.uiType = uiObjectType::Decoration;
            titleUI.align = Alignment::Center;
            titleUI.origin = { windowWidth / 2, windowHeight / 2 };
            titleUI.position = { titleUI.origin.x, titleUI.origin.y / 2 };
            DecorationProperties titleProperties;
            titleProperties.decType = DecorationType::Text;
            titleProperties.fontID = "PressStart2P";
            titleProperties.fontPath = "/font/PressStart2P-Regular.ttf";
            titleProperties.contents = "Untitled Game Project";
            titleProperties.fontSize = 48;
            titleProperties.fontColor = sf::Color{128,0,0,255};
            titleProperties.outlineColor = sf::Color::Black;
            titleProperties.outlineThickness = 1.0;
            UniqueID titleID("GameTitle");
            scene->uiProperties.insert(std::make_pair(titleID, titleUI));
            scene->decorationProperties.insert(std::make_pair(titleID, titleProperties));

            uiObjectProperties pressStartUI;
            pressStartUI.uiType = uiObjectType::Decoration;
            pressStartUI.align = Alignment::Center;
            pressStartUI.origin = { windowWidth / 2, windowHeight/2 };
            pressStartUI.position = { pressStartUI.origin.x, pressStartUI.origin.y + (3 * pressStartUI.origin.y / 4) };
            DecorationProperties pressStartProperties;
            pressStartProperties.decType = DecorationType::Text;
            pressStartProperties.fontID = "PressStart2P";
            pressStartProperties.fontPath = "/font/PressStart2P-Regular.ttf";
            pressStartProperties.contents = "Press any key to continue";
            pressStartProperties.fontSize = 36;
            pressStartProperties.fontColor = sf::Color{ 172, 172, 172, 255 };
            pressStartProperties.outlineColor = sf::Color::Black;
            pressStartProperties.outlineThickness = 1.0;
            UniqueID pressStartID("PressAnyKey");
            scene->uiProperties.insert(std::make_pair(pressStartID, pressStartUI));
            scene->decorationProperties.insert(std::make_pair(pressStartID, pressStartProperties));

            GameplayTransition* gameplay = new GameplayTransition(this);
            scene->keyListeners.insert(std::make_pair(pressStartID, gameplay));
            } break;
        case GameStatus::GamePlay:
            std::filesystem::path texturePath("/texture/oryx/oryx_16bit_fantasy_world.png");
            int mapWidth(40);
            int mapHeight(24);
            std::string fullPath(resourceSystem->GetResourceDirectory());
            fullPath.append(texturePath);
            Map* map = GenerateMap(fullPath, mapWidth, mapHeight);
            scene->map.reset(std::move(map));
            for(int y = 0; y < mapHeight; ++y) {
                for(int x = 0; x < mapWidth; ++x) {
                    sf::Texture* texture(nullptr);
                    Tile& tile(scene->map->tileArray[y][x]);
                    if(tile.terrainType == TerrainType::Floor) {
                        texture = resourceSystem->GetTexture("FloorTexture");
                    }
                    else if(tile.terrainType == TerrainType::Wall) {
                        texture = resourceSystem->GetTexture("WallTexture");
                    }
                    else {
                        texture = nullptr;
                    }
                    if(texture) {
                        sf::Sprite* sprite(CreateSprite(texture, scene));
                        sprite->setPosition(x * scene->map->properties.textureWidth, y * scene->map->properties.textureHeight);
                    }
                }
            }
            break;
    }
    return scene;
}

void Game::TransitionTo(Scene* scene) {
    InputSystem* inputSystem(GetInputSystem());

    if(currentScene) {
        for(auto iterator = scene->keyListeners.begin(); iterator != scene->keyListeners.end(); ++iterator) {
            inputSystem->RemoveListener(iterator->second, ListenerType::KeyPressListener);
        } // for each inputListener in Scene

        Scene* oldScene = currentScene.release();
        delete oldScene;
        currentScene.reset(scene);
    }
    else {
        currentScene.reset(scene);
    }
    for(auto iterator = scene->uiProperties.begin(); iterator != scene->uiProperties.end(); ++iterator) {
        if((*iterator).second.uiType == uiObjectType::Decoration) {
            auto uiIterator(scene->uiProperties.find((*iterator).first));
            auto decIterator(scene->decorationProperties.find((*iterator).first));
            if(uiIterator != scene->uiProperties.end() && decIterator != scene->decorationProperties.end()) {
                uiObjectProperties uiProperties = (*uiIterator).second;
                DecorationProperties decProperties = (*decIterator).second;
                Decoration* decPtr = CreateDecoration((*iterator).first, uiProperties, decProperties);
                std::unique_ptr<Decoration> decoration(decPtr);
                currentScene->uiObjects.push_back(std::move(decoration));
            }
        }
    } // for each uiProperties in Scene

    for(auto iterator = scene->keyListeners.begin(); iterator != scene->keyListeners.end(); ++iterator) {
        inputSystem->AddListener(iterator->second, ListenerType::KeyPressListener);
    } // for each inputListener in Scene
}

Map* Game::GenerateMap(std::filesystem::path textureSource, int width, int height) {
    Map* map(nullptr);
    if(width <= MaxMapWidth && height <= MaxMapHeight) {
        map = new Map;
        map->properties.width = width;
        map->properties.height = height;
        map->properties.texturePath = textureSource;
        map->properties.textureWidth = 24;
        map->properties.textureHeight = 24;
        map->properties.floorTexturePos = {408, 600};
        map->properties.wallTexturePos = {240, 288};
        UniqueID floorTextureID("FloorTexture");
        GetResourceSystem()->LoadTexture(floorTextureID,
                                         map->properties.texturePath,
                                         map->properties.floorTexturePos,
                                         map->properties.textureWidth,
                                         map->properties.textureHeight);
        UniqueID wallTextureID("WallTexture");
        GetResourceSystem()->LoadTexture(wallTextureID,
                                        map->properties.texturePath,
                                     map->properties.wallTexturePos,
                                       map->properties.textureWidth,
                                       map->properties.textureHeight);
        for(int y = 0; y < height; ++y) {
            for(int x = 0; x < width; ++x) {
                Tile& tile(map->tileArray[y][x]);
                tile.terrainType = TerrainType::Floor;
                tile.isWalkable = true;
                tile.isVisible = true;
            }
        }
    }
    return map;
}

void Game::AddFrameSegment(Decoration* frame, FrameSegment segmentID) {
    Position position{0,0};
    Position origin(frame->uiProperties.origin);
    sf::Vector2u windowSize{ (unsigned int)displayConfig.windowProperties.width,
                             (unsigned int)displayConfig.windowProperties.height - displayConfig.windowHeightModifier };
    sf::Vector2u textureSize{ (unsigned int)(frame->uiProperties.textureSource.width * displayConfig.uiScaleX) / 3,
                              (unsigned int)(frame->uiProperties.textureSource.height * displayConfig.uiScaleY) / 3 };
    DisplaySystem* displaySystem(GetDisplaySystem());
    sf::Sprite* sprite(nullptr);
    Decoration* segment(nullptr);
    UniqueID childID(frame->id + FrameSegmentNames.at((int)segmentID));
    if(segmentID == FrameSegment::TopLeft ||
        segmentID == FrameSegment::TopRight ||
        segmentID == FrameSegment::BottomLeft ||
        segmentID == FrameSegment::BottomRight) {
        if(segmentID == FrameSegment::TopLeft) {
            position = origin;
        } // TopLeft
        else if(segmentID == FrameSegment::TopRight) {
            position = { (int)windowSize.x - (int)textureSize.x, origin.y };
        } // TopRight
        else if(segmentID == FrameSegment::BottomLeft) {
            position = { origin.x, (int)windowSize.y - (int)textureSize.y };
        } // BottomLeft
        else if(segmentID == FrameSegment::BottomRight) {
            position = { (int)windowSize.x - (int)textureSize.x,
                         (int)windowSize.y - (int)textureSize.y };
        } // BottomRight
        sprite = CreateSprite(frame->texture, currentScene.get());
        sprite->setPosition(position.x, position.y);
        segment = new Decoration(childID, DecorationType::Frame );
        frame->children.push_back(segment);
    } // Corners only need 1 segment
    else if(segmentID == FrameSegment::TopMid) {
        position = { origin.x + (int)textureSize.x, origin.y };
        int segmentCount = ((int)windowSize.x / (int)textureSize.x) - 2;
        for(int segmentIndex = 0; segmentIndex < segmentCount; ++segmentIndex) {
            sprite = CreateSprite(frame->texture, currentScene.get());
            sprite->setPosition(position.x, position.y);
            segment = new Decoration(childID, DecorationType::Frame);
            frame->children.push_back(segment);
            position.x += textureSize.x;
        }
    } // TopMid
    else if(segmentID == FrameSegment::BottomMid) {
        position = { origin.x + (int)textureSize.x, (int)windowSize.y - (int)textureSize.y };
        int segmentCount = ((int)windowSize.x / (int)textureSize.x) - 2;
        for(int segmentIndex = 0; segmentIndex < segmentCount; ++segmentIndex) {
            sprite = CreateSprite(frame->texture, currentScene.get());
            sprite->setPosition(position.x, position.y);
            segment = new Decoration(childID, DecorationType::Frame);
            frame->children.push_back(segment);
            position.x += textureSize.x;
        }
    } // BottomMid
    else if(segmentID == FrameSegment::MidLeft) {
        position = { origin.x, origin.y + (int)textureSize.y };
        int segmentCount = ((int)windowSize.y / (int)textureSize.y) - 2;
        for(int segmentIndex = 0; segmentIndex < segmentCount; ++segmentIndex) {
            sprite = CreateSprite(frame->texture, currentScene.get());
            sprite->setPosition(position.x, position.y);
            segment = new Decoration(childID, DecorationType::Frame);
            frame->children.push_back(segment);
            position.y += textureSize.y;
        }
    } // MidLeft
    else if(segmentID == FrameSegment::MidRight) {
        position = { (int)windowSize.x - (int)textureSize.x, origin.y + (int)textureSize.y };
        int segmentCount = ((int)windowSize.y / (int)textureSize.y) - 2;
        for(int segmentIndex = 0; segmentIndex < segmentCount; ++segmentIndex) {
            sprite = CreateSprite(frame->texture, currentScene.get());
            sprite->setPosition(position.x, position.y);
            segment = new Decoration(childID, DecorationType::Frame);
            frame->children.push_back(segment);
            position.y += textureSize.y;
        }
    } // MidRight
    else if(segmentID == FrameSegment::Middle) {
        position = { origin.x + (int)textureSize.x, origin.y + (int)textureSize.y };
        int rowCount = (((int)windowSize.y) / (int)textureSize.y) - 2;
        int columnCount = ((int)windowSize.x / (int)textureSize.x) - 2;
        for(int y = 0; y < rowCount; ++y) {
            for(int x = 0; x < columnCount; ++x) {
                sprite = CreateSprite(frame->texture, currentScene.get());
                sprite->setPosition(position.x, position.y);
                segment = new Decoration(childID, DecorationType::Frame);
                frame->children.push_back(segment);
                position.x += textureSize.x;
            }
            position.x = origin.x + (int)textureSize.x;
            position.y += textureSize.y;
        }
    } // Middle
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

sf::Sprite* Game::CreateSprite(sf::Texture* texture, Scene* scene) {
    sf::Sprite* sprite(new sf::Sprite);
    sprite->setTexture(*texture);
    sprite->setScale(displayConfig.uiScaleX, displayConfig.uiScaleY);
    scene->spriteList.push_back(sprite);
    return sprite;
}

sf::Text* Game::CreateText(sf::Font* font, Scene* scene) {
    sf::Text* text(new sf::Text);
    text->setFont(*font);
    text->setScale(displayConfig.uiScaleX, displayConfig.uiScaleY);
    scene->textList.push_back(text);
    return text;
}
