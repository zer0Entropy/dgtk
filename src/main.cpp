#include <filesystem>
#include <SFML/Graphics.hpp>
#include "../include/display.hpp"
#include "../include/resource.hpp"
#include "../include/decoration.hpp"

int main()
{
    DisplayManager displayMgr;
    WindowProperties windowProperties;
    windowProperties.width = 1920;
    windowProperties.height = 1080;
    windowProperties.title = "Test Project";
    displayMgr.InitWindow(windowProperties);

    std::filesystem::path binDir(std::filesystem::current_path());
    std::filesystem::path runDir(binDir.parent_path());
    std::filesystem::path rootDir(runDir.parent_path());

    UniqueID backgroundID("background");
    std::string backgroundPath(rootDir);
    backgroundPath.append("/resource/texture/oryx_16bit_fantasy.jpg");
    ResourceManager resourceMgr;
    resourceMgr.LoadResource(backgroundID, ResourceType::Texture, backgroundPath);

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