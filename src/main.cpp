#include <filesystem>
#include <SFML/Graphics.hpp>
#include "../include/display.hpp"
#include "../include/input.hpp"
#include "../include/log.hpp"
#include "../include/resource.hpp"
#include "../include/decoration.hpp"

#include "../include/game.hpp"

bool CreateWindowFrame(DisplaySystem& display, ResourceSystem& resourceSystem);
bool CreateGameTitle(DisplaySystem& display, ResourceSystem& resourceSystem);

int main()
{
    Game game(std::filesystem::current_path());
    game.Init();

    /*
    game.CreateWindowFrame();
    game.CreateGameTitle();
    */

    uiObjectProperties cfgFrame;
    cfgFrame.uiType = uiObjectType::Decoration;
    cfgFrame.textureSource.pathToFile = "/texture/oryx/oryx_16bit_fantasy_world.png";
    cfgFrame.textureSource.width = 72;
    cfgFrame.textureSource.height = 72;
    cfgFrame.textureSource.topLeft = { 192, 576 };
    cfgFrame.origin = {0, 0};
    DecorationProperties frameProperties;
    frameProperties.decType = DecorationType::Frame;
    UniqueID frameID("WindowFrame");
    game.CreateDecoration(frameID, cfgFrame, frameProperties);

    int windowWidth(game.GetDisplayConfig().windowProperties.width);
    int windowHeight(game.GetDisplayConfig().windowProperties.height - game.GetDisplayConfig().windowHeightModifier);

    uiObjectProperties cfgTitle;
    cfgTitle.uiType = uiObjectType::Decoration;
    cfgTitle.align = Alignment::Center;
    cfgTitle.origin = { windowWidth / 2, windowHeight / 2 };
    cfgTitle.position = { cfgTitle.origin.x, cfgTitle.origin.y / 2 };
    DecorationProperties titleProperties;
    titleProperties.decType = DecorationType::Text;
    titleProperties.fontID = "PressStart2P";
    titleProperties.fontPath = "/font/PressStart2P-Regular.ttf";
    titleProperties.contents = "Untitled Game Project";
    titleProperties.fontSize = 24;
    titleProperties.fontColor = sf::Color{128,0,0,255};
    titleProperties.outlineColor = sf::Color::Black;
    titleProperties.outlineThickness = 0.4;
    UniqueID titleID("GameTitle");
    game.CreateDecoration(titleID, cfgTitle, titleProperties);

    auto displaySystem(game.GetDisplaySystem());
    auto window(displaySystem->GetWindow());

    while(window->isOpen()) {
        game.Update();
    }

    game.Shutdown();
}

