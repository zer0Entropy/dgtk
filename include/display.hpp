//
// Created by zeroc00l on 10/30/23.
//

#include <memory>
#include <string>
#include <vector>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>

#ifndef DGTKPROJECT_DISPLAY_HPP
#define DGTKPROJECT_DISPLAY_HPP

struct WindowProperties {
    std::string     title;
    int             width;
    int             height;
};

class DisplayManager {
public:
    void                        InitWindow(WindowProperties properties);
    void                        CloseWindow();
    sf::RenderWindow*           GetWindow() const;
    void                        Update();

    sf::Sprite*                 CreateSprite(sf::Texture* texture);
private:
    std::unique_ptr<sf::RenderWindow>           window;
    std::vector<sf::Sprite*>                    spriteList;
};

#endif //DGTKPROJECT_DISPLAY_HPP
