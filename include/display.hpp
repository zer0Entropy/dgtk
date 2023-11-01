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
#include "system.hpp"

#ifndef DGTKPROJECT_DISPLAY_HPP
#define DGTKPROJECT_DISPLAY_HPP

struct WindowProperties {
    std::string     title;
    int             width;
    int             height;
};

struct DisplayConfig {
    WindowProperties windowProperties;
    int             windowHeightModifier;
    float           uiScaleX;
    float           uiScaleY;
};

class Scene;
class Game;

class DisplaySystem: public System {
public:
    explicit DisplaySystem(const DisplayConfig& config, Game* gamePtr);
    DisplaySystem(const DisplaySystem& copy) = delete;
    ~DisplaySystem();

    void                        Init();
    void                        Update();
    void                        Shutdown();


    void                        InitWindow(WindowProperties properties);
    void                        CloseWindow();
    sf::RenderWindow*           GetWindow() const;

    void                        DrawScene(Scene* scene);
    std::pair<float,float>      GetUIScale() const;
private:
    Game*                                       game;
    std::unique_ptr<sf::RenderWindow>           window;
    WindowProperties                            windowProperties;
    float                                       uiScaleX, uiScaleY;
};

#endif //DGTKPROJECT_DISPLAY_HPP
