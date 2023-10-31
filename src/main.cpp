#include <filesystem>
#include <SFML/Graphics.hpp>
#include "../include/display.hpp"
#include "../include/resource.hpp"
#include "../include/decoration.hpp"

void InitDisplayManager(DisplayManager& displayMgr, const WindowProperties& properties);
bool InitResourceManager(ResourceManager& resourceMgr, std::filesystem::path workingDirectory);

bool InitWindowFrame(DisplayManager& displayMgr, ResourceManager& resourceMgr);

int main()
{
    DisplayManager displayMgr;

    WindowProperties windowProperties;
    windowProperties.width = 1920;
    windowProperties.height = 1080;
    windowProperties.title = "Test Project";
    InitDisplayManager(displayMgr, windowProperties);

    ResourceManager resourceMgr;
    bool success = InitResourceManager(resourceMgr, std::filesystem::current_path());
    if(!success) {
        return -1;
    }

    InitWindowFrame(displayMgr, resourceMgr);

    sf::RenderWindow* window(displayMgr.GetWindow());
    while (window->isOpen())
    {
        for (auto event = sf::Event{}; window->pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                displayMgr.CloseWindow();
            }
        }

        displayMgr.Update();
    }
}

void InitDisplayManager(DisplayManager& displayMgr, const WindowProperties& properties) {
    displayMgr.InitWindow(properties);
}

bool InitResourceManager(ResourceManager& resourceMgr, std::filesystem::path workingDirectory) {
    bool success = resourceMgr.SetWorkingDirectory(workingDirectory);
    return success;
}

bool InitWindowFrame(DisplayManager& displayMgr, ResourceManager& resourceMgr) {
    bool success(false);

    //sf::Vector2u windowSize(displayMgr.GetWindow()->getSize());
    const std::pair<float,float> scaleFactor(displayMgr.GetUIScale());
    const sf::Vector2u windowSize{ 1920, 1008 };
    int textureWidth(24);
    int textureHeight(24);
    Position topLeftPos{192, 576};

    UniqueID frameID("windowFrame");
    std::string framePath(resourceMgr.GetResourceDirectory());
    framePath.append("/texture/oryx/oryx_16bit_fantasy_world.png");
    resourceMgr.LoadFrameTextures(frameID, framePath, topLeftPos, textureWidth, textureHeight);

    textureWidth *= scaleFactor.first;
    textureHeight *= scaleFactor.second;

    for(int segmentIndex = 0; segmentIndex < (int)FrameSegment::TotalNumFrameSegments; ++segmentIndex) {
        UniqueID segmentID(frameID);
        segmentID.append(FrameSegmentNames.at(segmentIndex));
        Decoration frameSegment(segmentID, DecorationType::Frame);
        frameSegment.texture = resourceMgr.GetTexture(segmentID);
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
                    sprite = displayMgr.CreateSprite(frameSegment.texture);
                    sprite->setPosition((float)framePos.x, (float)framePos.y);
                    framePos.x += textureWidth;
                }
                framePos.x = origin.x + textureWidth;
                framePos.y += textureHeight;
            }
        }
        else {
            for (int spriteIndex = 0; spriteIndex < spriteCount; ++spriteIndex) {
                sprite = displayMgr.CreateSprite(frameSegment.texture);
                sprite->setPosition((float)framePos.x, (float)framePos.y);
                framePos.x += step.x;
                framePos.y += step.y;
            }
        }
    }

    return success;
}
