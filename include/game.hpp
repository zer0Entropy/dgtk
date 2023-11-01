//
// Created by zeroc00l on 10/31/23.
//

#include <filesystem>
#include <memory>
#include "system.hpp"
#include "display.hpp"
#include "scene.hpp"
#include "decoration.hpp"

#ifndef DGTKPROJECT_GAME_HPP
#define DGTKPROJECT_GAME_HPP

class LogSystem;
class ResourceSystem;
class InputSystem;

enum class GameStatus {
    Error = -1,
    MainMenu = 0,
    GamePlay = 1
};

class Game {
public:
    explicit Game(const std::filesystem::path& workingDir);
    Game(const Game& copy) = delete;
    ~Game();

    void    Init();
    void    Update();
    void    Shutdown();

    GameStatus  GetCurrentStatus() const;

    void        CreateGameTitle();

    Decoration*         CreateDecoration(UniqueID id, const uiObjectProperties& uiProperties, const DecorationProperties& properties);

    DisplaySystem*      GetDisplaySystem() const;
    InputSystem*        GetInputSystem() const;
    LogSystem*          GetLogSystem() const;
    ResourceSystem*     GetResourceSystem() const;

    const DisplayConfig& GetDisplayConfig() const;

    Scene*      GenerateScene(GameStatus nextStatus);
private:
    void        AddFrameSegment(Decoration* frame, FrameSegment segment);

    bool        FindGameConfig();

    bool        LoadGameConfig();

    GameStatus                  status;

    std::unique_ptr<System>     systems[(int)SystemID::TotalNumSystems];

    std::filesystem::path       workingDirectory;
    std::filesystem::path       projectRoot;
    std::filesystem::path       resourceDirectory;
    std::filesystem::path       configDirectory;
    std::filesystem::path       configFilePath;

    DisplayConfig               displayConfig;

    static const std::string    configFilename;
};

#endif //DGTKPROJECT_GAME_HPP
