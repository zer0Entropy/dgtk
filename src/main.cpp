#include <filesystem>
#include <SFML/Graphics.hpp>
#include "../include/resource.hpp"
#include "../include/decoration.hpp"

int main()
{
    auto window = sf::RenderWindow{ { 1920u, 1080u }, "CMake SFML Project" };
    window.setFramerateLimit(144);

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

    sf::Sprite backgroundSprite;
    backgroundSprite.setTexture(*background.texture);
    backgroundSprite.setPosition(0.0f, 0.0f);

    while (window.isOpen())
    {
        for (auto event = sf::Event{}; window.pollEvent(event);)
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }
        }

        window.clear();
        window.draw(backgroundSprite);
        window.display();
    }
}