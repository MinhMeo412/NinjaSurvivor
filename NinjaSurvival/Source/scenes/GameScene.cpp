#include "Utils.h"
#include "GameScene.h"
#include "gameUI/GameSceneUILayer.h"

using namespace ax;

GameScene::GameScene() : SceneBase("GameScene") {}

bool GameScene::init()
{
    if (!SceneBase::init())
    {
        return false;
    }

    uiLayer = GameSceneUILayer::create();
    this->addChild(uiLayer, 10);


    //Khởi tạo object GameWorld
    gameWorld = std::make_unique<GameWorld>();


    //Khởi tạo systemManager
    systemManager = SystemManager::getInstance();

    //Gọi init các system
    systemManager->initSystems(this, gameWorld.get(), uiLayer);

    //Update
    systemManager->update(0.0f); //Force update 1 lần
    uiLayer->update(0.0f); //Force update 1 lần

    this->scheduleOnce(AX_SCHEDULE_SELECTOR(GameScene::startUpdate), 1.5f); //Delay 1s = với loading scene

    return true;
}

//Đổi trạng thái và cho phép update
void GameScene::startUpdate(float dt)
{
    isUpdateStarted = true;
    scheduleUpdate();
}

void GameScene::update(float dt)
{
    if (!isUpdateStarted)
    {
        return;
    }

    systemManager->update(dt);
    uiLayer->update(dt);
}

