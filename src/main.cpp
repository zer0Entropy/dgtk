#include <filesystem>
#include <SFML/Graphics.hpp>
#include "../include/display.hpp"
#include "../include/resource.hpp"
#include "../include/decoration.hpp"

void InitDisplayManager(DisplayManager& displayMgr, const WindowProperties& properties);
bool InitResourceManager(ResourceManager& resourceMgr, std::filesystem::path workingDirectory);

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

    int textureWidth(24);
    int textureHeight(24);
    Position topLeftPos{193, 577};

    UniqueID backgroundID("background");
    std::string backgroundPath(resourceMgr.GetResourceDirectory());
    backgroundPath.append("/texture/oryx/oryx_16bit_fantasy_world.png");
    //resourceMgr.LoadResource(backgroundID, ResourceType::Texture, backgroundPath);
    resourceMgr.LoadTexture(backgroundID, backgroundPath, topLeftPos, textureWidth, textureHeight);

    Decoration background(backgroundID, DecorationType::Background);
    background.texture = resourceMgr.GetTexture(backgroundID);

    sf::Sprite* backgroundSprite = displayMgr.CreateSprite(background.texture);
    backgroundSprite->setPosition(0.0f, 0.0f);

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