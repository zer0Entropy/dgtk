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
    cfgFrame.textureSource.pathToFile = "/texture/oryx/oryx_16bit_fantasy_world.png";
    cfgFrame.textureSource.width = 72;
    cfgFrame.textureSource.height = 72;
    cfgFrame.textureSource.topLeft = { 192, 576 };
    cfgFrame.origin = {0, 0};
    DecorationProperties frameProperties;
    frameProperties.decType = DecorationType::Frame;
    UniqueID frameID("WindowFrame");
    game.CreateDecoration(frameID, cfgFrame, frameProperties);

    auto displaySystem(game.GetDisplaySystem());
    auto window(displaySystem->GetWindow());

    while(window->isOpen()) {
        game.Update();
    }

    game.Shutdown();
}

