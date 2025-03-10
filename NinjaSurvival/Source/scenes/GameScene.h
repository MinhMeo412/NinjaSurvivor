#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "axmol.h"
#include "SceneBase.h"

#include "systems/SystemManager.h"
#include "systems/GameWorld.h"

class GameScene : public SceneBase
{
public:
    GameScene();

    bool init() override;
    void update(float dt) override;

    void menuCloseCallback(ax::Object* sender);

private:
    ax::MenuItemImage* closeItem;


    SystemManager* systemManager = nullptr;
    std::unique_ptr<GameWorld> gameWorld;
};

#endif  // __GAME_SCENE_H__
