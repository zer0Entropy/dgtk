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

void Game::CreateWindowFrame() {
    bool success(false);
    DisplaySystem* displaySystem = static_cast<DisplaySystem*>(systems[(int)SystemID::Display].get());
    ResourceSystem* resourceSystem = static_cast<ResourceSystem*>(systems[(int)SystemID::Resource].get());

    int textureWidth(24);
    int textureHeight(24);
    Position topLeftPos{192, 576};

    UniqueID frameID("windowFrame");
    std::string framePath(resourceSystem->GetResourceDirectory());
    framePath.append("/texture/oryx/oryx_16bit_fantasy_world.png");
    resourceSystem->LoadFrameTextures(frameID, framePath, topLeftPos, textureWidth, textureHeight);

    textureWidth *= displayConfig.uiScaleX;
    textureHeight *= displayConfig.uiScaleY;

    sf::Vector2u windowSize(displayConfig.windowProperties.width, displayConfig.windowProperties.height - displayConfig.windowHeightModifier);

    for(int segmentIndex = 0; segmentIndex < (int)FrameSegment::TotalNumFrameSegments; ++segmentIndex) {
        UniqueID segmentID(frameID);
        segmentID.append(FrameSegmentNames.at(segmentIndex));
        Decoration frameSegment(segmentID, DecorationType::Frame);
        frameSegment.texture = resourceSystem->GetTexture(segmentID);
        Position origin{ 0, 0 };
        Position framePos(origin);
        Position step{0, 0};
        sf::Sprite* sprite(nullptr);
        int spriteCount(0);
        switch((FrameSegment)segmentIndex) {
            case FrameSegment::TopLeft:
                framePos = origin;
                spriteCount = 1;
                break;
            case FrameSegment::TopMid:
                framePos = { origin.x + textureWidth, origin.y };
                spriteCount = (int)(windowSize.x / textureWidth) - 2;
                step = { textureWidth, 0 };
                break;
            case FrameSegment::TopRight:
                framePos = { (int)windowSize.x - textureWidth + origin.x, origin.y };
                spriteCount = 1;
                break;
            case FrameSegment::MidLeft:
                framePos = {origin.x, origin.y + textureHeight};
                spriteCount = (int)(windowSize.y / textureHeight) - 2;
                step = { 0, textureHeight };
                break;
            case FrameSegment::Middle:
                framePos = {origin.x + textureWidth, origin.y + textureHeight };
                spriteCount = ((int)(windowSize.x / textureWidth) - 2) *
                              ((int)(windowSize.y / textureHeight) - 2);
                step = { 0, 0 };
                break;
            case FrameSegment::MidRight:
                framePos = { (int)windowSize.x - textureWidth + origin.x, origin.y + textureHeight };
                spriteCount = (int)(windowSize.y / textureHeight) - 2;
                step = { 0, textureHeight };
                break;
            case FrameSegment::BottomLeft:
                framePos = { origin.x, (int)windowSize.y - textureHeight + origin.y };
                spriteCount = 1;
                step = { 0, textureHeight };
                break;
            case FrameSegment::BottomMid:
                framePos = { origin.x + textureWidth, (int)windowSize.y - textureHeight + origin.y };
                spriteCount = (int)(windowSize.x / textureWidth) - 2;
                step = { textureWidth, 0 };
                break;
            case FrameSegment::BottomRight:
                framePos = { (int)windowSize.x - textureWidth + origin.x, (int)windowSize.y - textureHeight + origin.y };
                spriteCount = 1;
                step = { 0, textureHeight };
                break;
        }
        if(segmentIndex == (int)FrameSegment::Middle) {
            for(int colIndex = 0; colIndex < (int)(windowSize.y / textureHeight) - 2; ++colIndex) {
                for(int rowIndex = 0; rowIndex < (int)(windowSize.x / textureWidth) - 2; ++rowIndex) {
                    sprite = displaySystem->CreateSprite(frameSegment.texture);
                    sprite->setPosition((float)framePos.x, (float)framePos.y);
                    framePos.x += textureWidth;
                }
                framePos.x = origin.x + textureWidth;
                framePos.y += textureHeight;
            }
        }
        else {
            for (int spriteIndex = 0; spriteIndex < spriteCount; ++spriteIndex) {
                sprite = displaySystem->CreateSprite(frameSegment.texture);
                sprite->setPosition((float)framePos.x, (float)framePos.y);
                framePos.x += step.x;
                framePos.y += step.y;
            }
        }
    }
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
