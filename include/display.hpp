//
// Created by zeroc00l on 10/30/23.
//

#include <memory>
#include <string>
#include <vector>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

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
    sf::Text*                   CreateText(sf::Font* font);
    std::pair<float,float>      GetUIScale() const;
private:
    std::unique_ptr<sf::RenderWindow>           window;
    std::vector<sf::Sprite*>                    spriteList;
    std::vector<sf::Text*>                      textList;

    float       uiScaleX, uiScaleY;
};

#endif //DGTKPROJECT_DISPLAY_HPP
