//
// Created by zeroc00l on 10/31/23.
//

#include "../include/game.hpp"
#include "../include/log.hpp"
#include "../include/input.hpp"
#include "../include/resource.hpp"
#include "../include/decoration.hpp"

const std::string Game::configFilename = "game.cfg";

Game::Game(const std::filesystem::path& workingDir): workingDirectory(workingDir), status(GameStatus::MainMenu) {
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
                systems[systemIndex] = std::make_unique<DisplaySystem>(displayConfig);
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

void Game::CreateGameTitle() {
    DisplaySystem* displaySystem(static_cast<DisplaySystem*>(systems[(int)SystemID::Display].get()));
    ResourceSystem* resourceSystem(static_cast<ResourceSystem*>(systems[(int)SystemID::Resource].get()));
    UniqueID fontID("PressStartFont");
    std::filesystem::path resourcePath(resourceSystem->GetResourceDirectory());
    std::string fontPath(resourcePath);
    fontPath.append("/font/PressStart2P-Regular.ttf");
    resourceSystem->LoadResource(fontID, ResourceType::Font, fontPath);
    sf::Font* font(resourceSystem->GetFont(fontID));
    const int fontSize(24);

    sf::Vector2u windowSize(displaySystem->GetWindow()->getSize());
    std::string titleString("Untitled Game Project");
    sf::Color titleColor{ 128, 0, 0, 255 };
    Position titlePos{ ((int)windowSize.x / 2) - (int)(titleString.length() * (fontSize * displayConfig.uiScaleX / 2)),
                       ((int)windowSize.y / 3) - (int)(fontSize * displayConfig.uiScaleY / 2) };
    sf::Text* title(displaySystem->CreateText(font));
    title->setString(titleString);
    title->setCharacterSize(fontSize);
    title->setFillColor(titleColor);
    title->setOutlineColor(sf::Color::Black);
    title->setOutlineThickness(0.4f);
    title->setPosition(titlePos.x, titlePos.y);
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
                text = displaySystem->CreateText(font);
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

Scene* Game::GenerateScene(GameStatus nextStatus) {
    Scene* scene(new Scene);
    switch(nextStatus) {
        case GameStatus::Error:
            break;
        case GameStatus::MainMenu: {
            uiObjectProperties frameProperties;
            frameProperties.uiType = uiObjectType::Decoration;
            frameProperties.textureSource.pathToFile = "/texture/oryx/oryx_16bit_fantasy_world.png";
            frameProperties.textureSource.topLeft = {192, 576};
            frameProperties.textureSource.width = frameProperties.textureSource.height = 72;
            scene->uiProperties.push_back(frameProperties);
            } break;
        case GameStatus::GamePlay:
            break;
    }
    return scene;
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
        sprite = displaySystem->CreateSprite(frame->texture);
        sprite->setPosition(position.x, position.y);
        segment = new Decoration(childID, DecorationType::Frame );
        frame->children.push_back(segment);
    } // Corners only need 1 segment
    else if(segmentID == FrameSegment::TopMid) {
        position = { origin.x + (int)textureSize.x, origin.y };
        int segmentCount = ((int)windowSize.x / (int)textureSize.x) - 2;
        for(int segmentIndex = 0; segmentIndex < segmentCount; ++segmentIndex) {
            sprite = displaySystem->CreateSprite(frame->texture);
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
            sprite = displaySystem->CreateSprite(frame->texture);
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
            sprite = displaySystem->CreateSprite(frame->texture);
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
            sprite = displaySystem->CreateSprite(frame->texture);
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
                sprite = displaySystem->CreateSprite(frame->texture);
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
