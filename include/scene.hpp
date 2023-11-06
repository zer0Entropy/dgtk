//
// Created by zeroc00l on 10/31/23.
//

#include <memory>
#include <map>
#include <nlohmann/json.hpp>
#include "ui.hpp"
#include "decoration.hpp"
#include "map.hpp"
#include "input.hpp"
#include "player.hpp"
#include "view.hpp"

#ifndef DGTKPROJECT_SCENE_HPP
#define DGTKPROJECT_SCENE_HPP

enum class SceneKeyID {
    Scene = 0, SceneID,
    UIObjects, UIObjectID, UIType,
    DecorationType,
    TexturePath, TextureWidth, TextureHeight, TexturePosition,
    Origin, Position, Alignment, LayerID,
    FontID, FontPath, FontSize,
    FontColor, OutlineColor, OutlineThickness,
    Contents,
    UIAction, UIActionType, UIActionTrigger,
    ResourcePath,
    TotalNumSceneKeys
};

const std::map<SceneKeyID,std::string> SceneKeys{
        {SceneKeyID::Scene, "Scene"},
        {SceneKeyID::SceneID, "scene_id"},
        {SceneKeyID::UIObjects, "ui_objects"},
        {SceneKeyID::UIObjectID, "object_id"},
        {SceneKeyID::UIType, "ui_type"},
        {SceneKeyID::DecorationType, "decoration_type"},
        {SceneKeyID::TexturePath, "texture_path"},
        {SceneKeyID::TextureWidth, "texture_width"},
        {SceneKeyID::TextureHeight, "texture_height"},
        {SceneKeyID::TexturePosition, "texture_position"},
        {SceneKeyID::Origin, "origin"},
        {SceneKeyID::Position, "position"},
        {SceneKeyID::Alignment, "alignment"},
        {SceneKeyID::LayerID, "layer_id"},
        {SceneKeyID::FontID, "font_id"},
        {SceneKeyID::FontPath, "font_path"},
        {SceneKeyID::FontSize, "font_size"},
        {SceneKeyID::FontColor, "font_color"},
        {SceneKeyID::OutlineColor, "outline_color"},
        {SceneKeyID::OutlineThickness, "outline_thickness"},
        {SceneKeyID::Contents, "contents"},
        {SceneKeyID::UIAction, "ui_action"},
        {SceneKeyID::UIActionType, "action_type"},
        {SceneKeyID::UIActionTrigger, "action_trigger"},
        {SceneKeyID::ResourcePath, "resource_path"}
};

struct Scene {
    std::string                                 id;
    std::vector<std::unique_ptr<uiObject>>      uiObjects;
    std::map<UniqueID, InputListener*>          keyListeners;
    std::unique_ptr<Map>                        map;

    std::vector<Creature*>                      creatures;
    MapView                                     view;

    std::vector<sf::Drawable*>                  drawLayers[(int)uiLayerID::TotalNumUILayers];
};

class Game;

SceneID GetSceneID(UniqueID sceneName);

Scene*  LoadScene(std::string_view pathToJSON, Game* game);

void    AddToScene(Game* game, Scene* scene, uiObject* uiObjectPtr);

void    AddFrameSegment(Game* game, Decoration* frame, FrameSegmentID segment);

class SceneTransition: public InputListener {
public:
    SceneTransition(uiActionTrigger triggerType, std::string_view pathToScene, Game* gamePtr);
    SceneTransition(const SceneTransition& copy) = default;
    ~SceneTransition() = default;
    void ReceiveInput(const sf::Event& event) override;
private:
    uiActionTrigger trigger;
    std::string scenePath;
    Game* game;
};

#endif //DGTKPROJECT_SCENE_HPP
