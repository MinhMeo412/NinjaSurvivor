#include "Utils.h"
#include "GameScene.h"
#include "gameUI/GameSceneUILayer.h"
#include "AudioManager.h"
#include "systems/ShopSystem.h"

using namespace ax;

GameScene::GameScene() : SceneBase("GameScene") {}

bool GameScene::init()
{
    if (!SceneBase::init())
    {
        return false;
    }

    // Khởi tạo nhạc nền cho game
    AudioManager::getInstance()->playSound("gamebackground_music", true, 0.5f, "gamebackground");

    uiLayer = GameSceneUILayer::create();
    this->addChild(uiLayer, 10);

    //Khởi tạo object GameWorld
    gameWorld = std::make_unique<GameWorld>();


    //Khởi tạo systemManager
    systemManager = SystemManager::getInstance();

    //Gọi init các system
    systemManager->initSystems(this, gameWorld.get(), uiLayer);

    ShopSystem::getInstance()->syncRerollCountWithLevelSystem();

    //Update
    //for (int i = 0; i < 10; i++)
    {
        systemManager->update(0.0f);  // Force update 1 lần
    }
    uiLayer->update(0.0f);  // Force update 1 lần

    this->scheduleOnce(AX_SCHEDULE_SELECTOR(GameScene::startUpdate), 1.0f); //Delay 1s

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

