//
// Created by zeroc00l on 10/30/23.
//

#include <map>
#include <string>

#ifndef DGTKPROJECT_ID_HPP
#define DGTKPROJECT_ID_HPP

typedef std::string UniqueID;

enum class SceneID {
    MainMenu = 0, Gameplay, TotalNumSceneIDs
};

const std::map<SceneID,std::string> SceneNames{
        {SceneID::MainMenu, "MainMenu"},
        {SceneID::Gameplay, "Gameplay"}
};

#endif //DGTKPROJECT_ID_HPP
