//
// Created by zeroc00l on 10/31/23.
//

#include <filesystem>
#include <memory>
#include "system.hpp"
#include "display.hpp"
#include "scene.hpp"
#include "decoration.hpp"
#include "map.hpp"
#include "player.hpp"
#include "bsp.hpp"

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
    explicit Game(const std::filesystem::path& workingDir, unsigned int seed);
    Game(const Game& copy) = delete;
    ~Game();

    void    Init();
    void    Update();
    void    Shutdown();

    GameStatus  GetCurrentStatus() const;

    DisplaySystem*      GetDisplaySystem() const;
    InputSystem*        GetInputSystem() const;
    LogSystem*          GetLogSystem() const;
    ResourceSystem*     GetResourceSystem() const;

    const DisplayConfig& GetDisplayConfig() const;

    MathParser&         GetMathParser() const;
    RandomNumberGenerator&  GetRNG() const;

    Scene*              GetCurrentScene() const;

    void                ApplyUIScaling(sf::Transformable* transform);
    void                ApplyTileScaling(sf::Transformable* transform);

    void                TransitionTo(Scene* scene);

    Creature*           FindCreature(UniqueID id);

    bool                MoveCreature(Creature* creature, MapLocation location);

    Decoration*         CreateFrameSegment(Decoration* frame, FrameSegmentID segmentID);
    Decoration*         CreateFrame(const uiObjectProperties& uiObjProperties, const DecorationProperties& decProperties);
    Decoration*         CreateDecoration(const uiObjectProperties& uiObjProperties, const DecorationProperties& decProperties);

    void                SignalInterrupt();

private:

    bool                FindGameConfig();

    bool                LoadGameConfig();

    Player*             CreatePlayer(std::string name, sf::Texture* texture, MapLocation location, const MapProperties& mapProperties);
    Creature*           CreateCreature(std::string name, sf::Texture* texture, MapLocation location, const MapProperties& mapProperties);

    GameStatus                  status;

    std::unique_ptr<Scene>      currentScene;

    std::unique_ptr<System>     systems[(int)SystemID::TotalNumSystems];

    std::filesystem::path       workingDirectory;
    std::filesystem::path       projectRoot;
    std::filesystem::path       resourceDirectory;
    std::filesystem::path       configDirectory;
    std::filesystem::path       configFilePath;

    DisplayConfig               displayConfig;

    MathParser                  mathParser;
    RandomNumberGenerator       rng;

    static const std::string    configFilename;
};

#endif //DGTKPROJECT_GAME_HPP
