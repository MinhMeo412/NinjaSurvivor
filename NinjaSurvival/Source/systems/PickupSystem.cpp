#include "PickupSystem.h"
#include "SpawnSystem.h"
#include "MovementSystem.h"
#include "LevelSystem.h"
#include "gameUI/GameSceneUILayer.h"
#include "gameUI/LevelUpOrChestEventLayer.h"
#include "scenes/GameScene.h"
#include "SystemManager.h"
#include "AudioManager.h"

PickupSystem::PickupSystem(EntityManager& em, ComponentManager<IdentityComponent>& im, ComponentManager<TransformComponent>& tm)
    : entityManager(em), identityMgr(im), transformMgr(tm)
{}

void PickupSystem::init()
{
    PICKUP_RANGE = 50.0f; //+buff
    TOUCH_RANGE  = 10.0f;

    pickupItems = {"greenGem", "blueGem", "redGem", "coin", "heart"};
    touchItems  = {"bomb", "magnet", "chest"};
}

void PickupSystem::update(float dt)
{
    auto start          = std::chrono::high_resolution_clock::now();

    auto spawnSystem    = SystemManager::getInstance()->getSystem<SpawnSystem>();
    auto movementSystem = SystemManager::getInstance()->getSystem<MovementSystem>();
    if (!spawnSystem || !movementSystem)
        return;

    ax::Vec2 playerPos       = spawnSystem->getPlayerPosition();

    for (auto entity : entityManager.getActiveEntities())
    {
        auto identity = identityMgr.getComponent(entity);
        if (!identity || identity->type != "item")
            continue;

        auto transform = transformMgr.getComponent(entity);
        if (!transform)
            continue;

        ax::Vec2 itemPos(transform->x, transform->y);
        float distance = playerPos.distance(itemPos);

        bool shouldPickup = false;
        if (pickupItems.count(identity->name))
        {
            shouldPickup = (distance <= PICKUP_RANGE);
        }
        else if (touchItems.count(identity->name))
        {
            shouldPickup = (distance <= TOUCH_RANGE);
        }

        if (shouldPickup && !movementSystem->isAnimating(entity))
        {
            movementSystem->moveItemToPlayer(entity);
        }
    }

   

    auto end      = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    //AXLOG("Thời gian thực thi PickupSystem: %ld ms", duration);
}

void PickupSystem::applyPickupEffect(std::string& itemName)
{
    if (itemName == "greenGem" || itemName == "blueGem" || itemName == "redGem")
    {
        AudioManager::getInstance()->playSound("exp", false, 1.0f, "exp");
        applyXPGem(itemName);
        return;
    }

    else if (itemName == "coin")
    {
        AudioManager::getInstance()->playSound("coin", false, 1.0f, "coin");
        applyCoin();
        return;
    }

    else if (itemName == "chest")
    {

        applyChest();
        return;
    }


}

void PickupSystem::applyHeart() {}
void PickupSystem::applyCoin()
{
    float amount   = 1;
    auto gameLayer = dynamic_cast<GameSceneUILayer*>(SystemManager::getInstance()->getSceneLayer());
    gameLayer->increaseCoin(amount);
}
void PickupSystem::applyXPGem(std::string& itemName)
{
    if (itemName == "greenGem")
    {
        SystemManager::getInstance()->getSystem<LevelSystem>()->increaseXP(1.0);
        return;
    }
    if (itemName == "blueGem")
    {
        SystemManager::getInstance()->getSystem<LevelSystem>()->increaseXP(3.0);
        return;
    }
    if (itemName == "redGem")
    {
        SystemManager::getInstance()->getSystem<LevelSystem>()->increaseXP(5.0);
        return;
    }
}
void PickupSystem::applyBomb() {}
void PickupSystem::applyMagnet() {}
void PickupSystem::applyChest()
{
    //std::vector<std::pair<std::string, int>> upgradeList = {{"sword", 1}};  // List này chỉ có 1 giá trị (nếu full hết thì cho giá trị coin)

    //auto gameScene = dynamic_cast<GameScene*>(SystemManager::getInstance()->getCurrentScene());
    //if (gameScene)
    //{
    //    auto levelUpLayer = LevelUpOrChestEventLayer::create(false, upgradeList);  // isLevelUp = false
    //    if (levelUpLayer)
    //    {
    //        // Lấy vị trí của uiLayer
    //        ax::Vec2 uiLayerPos = gameScene->getUILayer()->getPosition();
    //        levelUpLayer->setPosition(uiLayerPos);
    //        gameScene->addChild(levelUpLayer, 1000);  // Thêm layer
    //        gameScene->unscheduleUpdate();            // Dừng update
    //    }
    //}
}
