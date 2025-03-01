#ifndef __MAP_CHOOSE_SCENE_H__
#define __MAP_CHOOSE_SCENE_H__

#include "axmol.h"
#include "SceneBase.h"
#include "systems/SystemManager.h"

class MapChooseScene : public SceneBase
{
public:
    MapChooseScene();

    bool init() override;
    void update(float dt) override;


private:
    ax::MenuItemImage* closeItem = nullptr;
    ax::MenuItemSprite* playButton = nullptr;
    ax::MenuItemSprite* selectedMapItem = nullptr;
    std::string selectedMapName;

    SystemManager* systemManager = nullptr;

    // a selector callback
    void menuCloseCallback(ax::Object* sender);
    void menuPlayCallback(ax::Object* sender);
    void menuUISetup();
};

#endif  // __MAP_CHOOSE_SCENE_H__
