//
// Created by zeroc00l on 10/30/23.
//

#include <fstream>
#include <nlohmann/json.hpp>
#include "../include/resource.hpp"
#include "../include/decoration.hpp"
#include "../include/scene.hpp"
#include "../include/game.hpp"
#include "../include/log.hpp"

ResourceSystem::ResourceSystem(const std::filesystem::path& resourceDir):
    System(SystemID::Resource), resourceDirectory(resourceDir) {

}

ResourceSystem::~ResourceSystem() {

}

void ResourceSystem::Init() {
    bool success(LoadSceneIndex());
}

void ResourceSystem::Update() {

}

void ResourceSystem::Shutdown() {
    resourceMap.clear();
    textureMap.clear();
    fontMap.clear();
    soundMap.clear();
    musicMap.clear();
}

bool ResourceSystem::LoadResource(UniqueID id, ResourceType type, std::string_view path) {
    bool success(false);
    std::string fullPath(resourceDirectory);
    fullPath.append(path);

    if (type == ResourceType::Texture) {
        std::unique_ptr<sf::Texture> texture(LoadTexture(fullPath));
        if (texture) {
            success = true;
            textureMap.insert(std::make_pair(id, std::move(texture)));
        }
    } else if (type == ResourceType::Font) {
        std::unique_ptr<sf::Font> font(LoadFont(fullPath));
        if (font) {
            success = true;
            fontMap.insert(std::make_pair(id, std::move(font)));
        }
    } else if (type == ResourceType::Sound) {
        std::unique_ptr<sf::SoundBuffer> sound(LoadSound(fullPath));
        if (sound) {
            success = true;
            soundMap.insert(std::make_pair(id, std::move(sound)));
        }
    } else if (type == ResourceType::Music) {
        std::unique_ptr<sf::Music> music(LoadMusic(fullPath));
        if(music) {
            success = true;
            musicMap.insert(std::make_pair(id, std::move(music)));
    }
}

    ResourceHandle* resourceHandle = new ResourceHandle(id, type, fullPath);
    resourceMap.insert(std::make_pair(id, resourceHandle));
    return success;
}

bool ResourceSystem::LoadTexture(UniqueID id, std::string_view path, Position startPos, int width, int height) {
    bool success(false);
    std::unique_ptr<sf::Texture> texture(LoadTexture(path, startPos, width, height));
    if (texture) {
        success = true;
        textureMap.insert(std::make_pair(id, std::move(texture)));
    }
    return success;
}

bool ResourceSystem::LoadFrameTextures(UniqueID id, std::string_view path, Position topLeftPos, int segmentWidth, int segmentHeight) {
    bool success(false);
    Position currentPos(topLeftPos);
    std::string fullPath(resourceDirectory);
    fullPath.append(path);
    for(int segmentIndex = (int)FrameSegmentID::TopLeft; segmentIndex < (int)FrameSegmentID::TotalNumFrameSegmentIDs; ++segmentIndex) {
        auto texture = std::make_unique<sf::Texture>();
        success = texture->loadFromFile(fullPath, sf::IntRect(currentPos.x,
                                                                       currentPos.y,
                                                                       segmentWidth,
                                                                       segmentHeight));
        if(success) {
            UniqueID segmentID(id);
            segmentID.append(FrameSegmentNames.at(segmentIndex));
            textureMap.insert(std::make_pair(segmentID, std::move(texture)));
        }
        if(currentPos.x < (topLeftPos.x + (2 * segmentWidth))) {
            currentPos.x += segmentWidth;
        }
        else if(currentPos.y < (topLeftPos.y + (2 * segmentHeight))) {
            currentPos.x = topLeftPos.x;
            currentPos.y += segmentHeight;
        }
        else {
            break;
        }
    }
    return success;
}

bool ResourceSystem::LoadSceneIndex() {
    bool success(false);
    std::string path(resourceDirectory);
    path.append(SceneIndexPath);
    std::ifstream sceneIndex(path);
    if(sceneIndex.good()) {
        success = true;

        nlohmann::json jsonDoc = nlohmann::json::parse(sceneIndex);
        auto sceneIndexJSON = jsonDoc.begin().value();
        for(auto sceneIter = sceneIndexJSON.begin(); sceneIter != sceneIndexJSON.end(); ++sceneIter) {
            for(int index = 0; index < (int)SceneID::TotalNumSceneIDs; ++index) {
                SceneID sceneID((SceneID) index);
                std::string_view findKey(SceneNames.at(sceneID));
                std::string currentKey = sceneIter.key();
                if(currentKey.compare(findKey) == 0) {
                    std::string value = sceneIter.value();
                    scenePathMap.insert(std::make_pair(currentKey, value));
                }
            }
        }
        sceneIndex.close();
    }
    return success;
}

ResourceHandle* ResourceSystem::GetResourceHandle(UniqueID id) {
    ResourceHandle* resourceHandle(nullptr);
    auto iterator(resourceMap.find(id));
    if(iterator != resourceMap.end()) {
        resourceHandle = iterator->second.get();
    }
    return resourceHandle;
}

sf::Texture* ResourceSystem::GetTexture(UniqueID id) {
    sf::Texture* texture(nullptr);
    auto iterator(textureMap.find(id));
    if(iterator != textureMap.end()) {
        texture = iterator->second.get();
    }
    return texture;
}

sf::Font* ResourceSystem::GetFont(UniqueID id) {
    sf::Font* font(nullptr);
    auto iterator(fontMap.find(id));
    if(iterator != fontMap.end()) {
        font = iterator->second.get();
    }
    return font;
}

sf::SoundBuffer* ResourceSystem::GetSound(UniqueID id) {
    sf::SoundBuffer* sound(nullptr);
    auto iterator(soundMap.find(id));
    if(iterator != soundMap.end()) {
        sound = iterator->second.get();
    }
    return sound;
}

sf::Music* ResourceSystem::GetMusic(UniqueID id) {
    sf::Music* music(nullptr);
    auto iterator(musicMap.find(id));
    if(iterator != musicMap.end()) {
        music = iterator->second.get();
    }
    return music;
}

const std::filesystem::path& ResourceSystem::GetResourceDirectory() const {
    return resourceDirectory;
}

std::filesystem::path ResourceSystem::GetScenePath(UniqueID id) const {

    std::string scenePath(resourceDirectory);
    auto pathIterator(scenePathMap.find(id));
    if(pathIterator != scenePathMap.end()) {
        scenePath = scenePath + pathIterator->second;
    }
    return scenePath;
}

Scene* ResourceSystem::LoadScene(std::string_view path, Game& game) {
    Scene* scene(nullptr);

    nlohmann::json jsonDoc;
    std::ifstream jsonFile(std::string{path});

    if(!jsonFile.good()) {
        // TODO: Report error here!
        return scene;
    } else {
        jsonDoc = nlohmann::json::parse(jsonFile);
    }

    LogSystem* logSystem = game.GetLogSystem();
    scene = new Scene;
    SceneProperties sceneProperties(ReadScenePropertiesFromJSON(jsonDoc, &game));
    scene->properties = sceneProperties;

    if(jsonFile.is_open()) {
        jsonFile.close();
    }

    return scene;
}

std::unique_ptr<sf::Texture> ResourceSystem::LoadTexture(std::string_view path,
                                                          Position startPos,
                                                          int width,
                                                          int height) {
    auto texture = std::make_unique<sf::Texture>();
    bool success(false);
    std::string fullPath(resourceDirectory);
    fullPath.append(path);

    if(width == 0 && height == 0) {
        success = texture->loadFromFile(std::string(fullPath));
    }
    else {
        success = texture->loadFromFile(std::string(fullPath), sf::IntRect(startPos.x,
                                                                       startPos.y,
                                                                       width,
                                                                       height));
    }
    if(!success) {
        texture.release();
        texture = nullptr;
    }
    return std::move(texture);
}

std::unique_ptr<sf::Font> ResourceSystem::LoadFont(std::string_view path) {
    auto font = std::make_unique<sf::Font>();
    bool success = font->loadFromFile(std::string(path));
    if(!success) {
        font.release();
        font = nullptr;
    }
    return std::move(font);
}

std::unique_ptr<sf::SoundBuffer> ResourceSystem::LoadSound(std::string_view path) {
    auto sound = std::make_unique<sf::SoundBuffer>();
    bool success = sound->loadFromFile(std::string(path));
    if(!success) {
        sound.release();
        sound = nullptr;
    }
    return std::move(sound);
}

std::unique_ptr<sf::Music> ResourceSystem::LoadMusic(std::string_view path) {
    auto music = std::make_unique<sf::Music>();
    bool success = music->openFromFile(std::string(path));
    if(!success) {
        music.release();
        music = nullptr;
    }
    return std::move(music);
}

