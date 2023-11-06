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

void Game::ApplyUIScaling(sf::Transformable* transform) {
    transform->setScale(displayConfig.uiScaleX, displayConfig.uiScaleY);
}

void Game::ApplyTileScaling(sf::Transformable* transform) {
    transform->setScale(displayConfig.tileScaleX, displayConfig.tileScaleY);
}

void Game::TransitionTo(Scene* scene) {
    InputSystem* inputSystem(GetInputSystem());

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

    for(auto iterator = currentScene->keyListeners.begin(); iterator != currentScene->keyListeners.end(); ++iterator) {
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
                if(y == 0 || y == height - 1 || x == 0 || x == width - 1) {
                    tile.terrainType = TerrainType::Wall;
                    tile.isWalkable = false;
                } else {
                    tile.terrainType = TerrainType::Floor;
                    tile.isWalkable = true;
                }
                tile.isVisible = true;
                tile.creature = nullptr;
                tile.sprite.reset(new sf::Sprite);
            }
        }
    }
    return map;
}

bool Game::MoveCreature(Creature* creature, MapLocation location) {
    bool success(false);
    MapLocation oldLocation(creature->location);
    Map& map(*currentScene->map);
    Tile& oldTile(map.tileArray[oldLocation.y][oldLocation.x]);
    int mapWidth(map.properties.width);
    int mapHeight(map.properties.height);
    if(location.x >= 0 && location.x < mapWidth
        && location.y >= 0 && location.y < mapHeight) {
        Tile& newTile(map.tileArray[location.y][location.x]);
        if(newTile.isWalkable && !newTile.creature) {
            oldTile.creature = nullptr;
            newTile.creature = creature;
            creature->location = location;
            success = true;
        }
    } // check if location is valid
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

Decoration* Game::CreateDecoration(UniqueID id, DecorationType decType) {
    Decoration* decoration = nullptr;
    bool validType(false);
    switch(decType) {
        case DecorationType::Background:    validType = true;
            break;
        case DecorationType::Doodad:        validType = true;
            break;
        case DecorationType::Frame:         validType = true;
            break;
        case DecorationType::Text:          validType = true;
            break;
    }
    if(validType) {
        decoration = new Decoration(id, decType);
    }

    return decoration;
}

void Game::SignalInterrupt() {
    for(auto& system : systems) {
        system.get()->SetInterrupt();
    }
}

Player* Game::CreatePlayer(std::string name, sf::Texture* texture, MapLocation location, const MapProperties& mapProperties) {
    Player* player = new Player;
    player->character.reset(std::move(CreateCreature(name, texture, location, mapProperties)));
    return player;
}

Creature* Game::CreateCreature(std::string name, sf::Texture* texture, MapLocation location, const MapProperties& mapProperties) {
    Creature* creature = new Creature;
    creature->name = name;
    creature->texture = texture;
    creature->location = location;
    creature->position = { (int)(location.x * mapProperties.textureWidth * displayConfig.tileScaleX),
                           (int)(location.y * mapProperties.textureHeight * displayConfig.tileScaleY) };
    creature->sprite = nullptr;
    return creature;
}

void Game::InitMapView(MapView& view, Map* map) {
    int tileWidth(map->properties.textureWidth * displayConfig.tileScaleX);
    int tileHeight(map->properties.textureHeight * displayConfig.tileScaleY);
    view.widthInPixels = displayConfig.windowProperties.width;
    view.heightInPixels = displayConfig.windowProperties.height - displayConfig.windowHeightModifier;
    view.widthInTiles = view.widthInPixels / tileWidth;
    view.heightInTiles = view.heightInPixels / tileHeight;
}
