#include <filesystem>
#include <SFML/Graphics.hpp>
#include "../include/display.hpp"
#include "../include/input.hpp"
#include "../include/resource.hpp"
#include "../include/decoration.hpp"

bool CreateWindowFrame(DisplaySystem& display, ResourceSystem& resourceSystem);
bool CreateGameTitle(DisplaySystem& display, ResourceSystem& resourceSystem);

int main()
{
    DisplayConfig displayConfig;
    displayConfig.uiScaleX = displayConfig.uiScaleY = 2.0f;
    displayConfig.windowProperties.width = 1920;
    displayConfig.windowProperties.height = 1080;
    displayConfig.windowProperties.title = "Untitled Game Project";
    displayConfig.windowHeightModifier = -48;
    DisplaySystem displaySystem(displayConfig);
    ResourceSystem resourceSystem(std::filesystem::current_path());
    InputSystem inputSystem(&displaySystem);
    displaySystem.Init();
    resourceSystem.Init();
    inputSystem.Init();

    CreateWindowFrame(displaySystem, resourceSystem);
    CreateGameTitle(displaySystem, resourceSystem);

    sf::RenderWindow* window(displaySystem.GetWindow());
    while (window->isOpen()) {
        inputSystem.Update();
        displaySystem.Update();
        resourceSystem.Update();
    }
}

bool CreateWindowFrame(DisplaySystem& displaySystem, ResourceSystem& resourceSystem) {
    bool success(false);

    //sf::Vector2u windowSize(display.GetWindow()->getSize());
    const std::pair<float,float> scaleFactor(displaySystem.GetUIScale());
    const sf::Vector2u windowSize{ 1920, 1008 };
    int textureWidth(24);
    int textureHeight(24);
    Position topLeftPos{192, 576};

    UniqueID frameID("windowFrame");
    std::string framePath(resourceSystem.GetResourceDirectory());
    framePath.append("/texture/oryx/oryx_16bit_fantasy_world.png");
    resourceSystem.LoadFrameTextures(frameID, framePath, topLeftPos, textureWidth, textureHeight);

    textureWidth *= scaleFactor.first;
    textureHeight *= scaleFactor.second;

    for(int segmentIndex = 0; segmentIndex < (int)FrameSegment::TotalNumFrameSegments; ++segmentIndex) {
        UniqueID segmentID(frameID);
        segmentID.append(FrameSegmentNames.at(segmentIndex));
        Decoration frameSegment(segmentID, DecorationType::Frame);
        frameSegment.texture = resourceSystem.GetTexture(segmentID);
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
                    sprite = displaySystem.CreateSprite(frameSegment.texture);
                    sprite->setPosition((float)framePos.x, (float)framePos.y);
                    framePos.x += textureWidth;
                }
                framePos.x = origin.x + textureWidth;
                framePos.y += textureHeight;
            }
        }
        else {
            for (int spriteIndex = 0; spriteIndex < spriteCount; ++spriteIndex) {
                sprite = displaySystem.CreateSprite(frameSegment.texture);
                sprite->setPosition((float)framePos.x, (float)framePos.y);
                framePos.x += step.x;
                framePos.y += step.y;
            }
        }
    }

    return success;
}

bool CreateGameTitle(DisplaySystem& displaySystem, ResourceSystem& resourceSystem) {
    bool success(false);
    UniqueID fontID("PressStartFont");
    std::filesystem::path resourcePath(resourceSystem.GetResourceDirectory());
    std::string fontPath(resourcePath);
    fontPath.append("/font/PressStart2P-Regular.ttf");
    success = resourceSystem.LoadResource(fontID, ResourceType::Font, fontPath);
    sf::Font* font(resourceSystem.GetFont(fontID));
    const int fontSize(24);
    const std::pair<float,float> scaleFactor(displaySystem.GetUIScale());
    sf::Vector2u windowSize(displaySystem.GetWindow()->getSize());
    std::string titleString("Untitled Game Project");
    sf::Color titleColor{ 128, 0, 0, 255 };
    Position titlePos{ ((int)windowSize.x / 2) - (int)(titleString.length() * (fontSize * scaleFactor.first / 2)),
                       ((int)windowSize.y / 3) - (int)(fontSize * scaleFactor.second / 2) };
    sf::Text* title(displaySystem.CreateText(font));
    title->setString(titleString);
    title->setCharacterSize(fontSize);
    title->setFillColor(titleColor);
    title->setOutlineColor(sf::Color::Black);
    title->setOutlineThickness(0.4f);
    title->setPosition(titlePos.x, titlePos.y);
    return success;
}
