#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "axmol.h"
#include "SceneBase.h"
#include "gameUI/GameSceneUILayer.h"

#include "systems/SystemManager.h"
#include "systems/GameWorld.h"

class GameScene : public SceneBase
{
public:
    GameScene();

    bool init() override;
    void update(float dt) override;

    void startUpdate(float dt);

    ax::Layer* getUILayer() const { return uiLayer; }

private:
    ax::Layer* uiLayer = nullptr;

    SystemManager* systemManager = nullptr;
    std::unique_ptr<GameWorld> gameWorld;

    bool isUpdateStarted = false;
};

#endif  // __GAME_SCENE_H__
