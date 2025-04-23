#include "Utils.h"
#include "PickupSystem.h"
#include "ShopSystem.h"
#include "SpawnSystem.h"
#include "MovementSystem.h"
#include "LevelSystem.h"
#include "HealthSystem.h"
#include "gameUI/GameSceneUILayer.h"
#include "gameUI/LevelUpOrChestEventLayer.h"
#include "scenes/GameScene.h"
#include "SystemManager.h"
#include "AudioManager.h"

PickupSystem::PickupSystem(EntityManager& em,
                           ComponentManager<IdentityComponent>& im,
                           ComponentManager<TransformComponent>& tm)
    : entityManager(em), identityMgr(im), transformMgr(tm)
{}

void PickupSystem::init()
{
    PICKUP_RANGE   = 50.0f * (1 + ShopSystem::getInstance()->getShopBuff("LootRange")) * (1 + pickupMultiplier);
    TOUCH_RANGE    = 16.0f;
    coinMultiplier = 1 + ShopSystem::getInstance()->getShopBuff("CoinGain");

    pickupItems = {"greenGem", "blueGem", "redGem", "coin", "heart"};
    touchItems  = {"bomb", "magnet", "chest"};
}

void PickupSystem::update(float dt)
{
    auto start = std::chrono::high_resolution_clock::now();

    auto spawnSystem    = SystemManager::getInstance()->getSystem<SpawnSystem>();
    auto movementSystem = SystemManager::getInstance()->getSystem<MovementSystem>();
    if (!spawnSystem || !movementSystem)
        return;

    ax::Vec2 playerPos = spawnSystem->getPlayerPosition();

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
    // AXLOG("Thời gian thực thi PickupSystem: %ld ms", duration);
}

void PickupSystem::applyPickupEffect(std::string& itemName)
{
    if (itemName == "greenGem" || itemName == "blueGem" || itemName == "redGem")
    {
        AudioManager::getInstance()->playSound("exp", false, 1.0f, "item");
        applyXPGem(itemName);
        return;
    }

    else if (itemName == "coin")
    {
        AudioManager::getInstance()->playSound("coin", false, 0.5f, "item");
        applyCoin();
        return;
    }

    else if (itemName == "chest")
    {
        AudioManager::getInstance()->playSound("chest", false, 1.0f, "item");
        applyChest();
        return;
    }

    else if (itemName == "heart")
    {
        AudioManager::getInstance()->playSound("magnet_heart", false, 1.0f, "item");
        applyHeart();
        return;
    }

    else if (itemName == "magnet")
    {
        AudioManager::getInstance()->playSound("magnet_heart", false, 1.0f, "item");
        applyMagnet();
        return;
    }

    else if (itemName == "bomb")
    {
        AudioManager::getInstance()->playSound("bomb", false, 1.0f, "item");
        applyBomb();
        return;
    }
}

void PickupSystem::applyHeart()
{
    SystemManager::getInstance()->getSystem<HealthSystem>()->setPlayerCurrentHealth(25.0);
}

void PickupSystem::applyCoin()
{
    float amount   = 1 * coinMultiplier;
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

void PickupSystem::applyBomb()
{
    SystemManager::getInstance()->getSystem<HealthSystem>()->applyBombDamageToAll(100);
}

void PickupSystem::applyMagnet()
{
    for (auto entity : entityManager.getActiveEntities())
    {
        auto identity = identityMgr.getComponent(entity);
        if (!identity || identity->type != "item")
            continue;
        if (Utils::in(identity->name, "greenGem", "blueGem", "redGem"))
        {
            auto movementSystem = SystemManager::getInstance()->getSystem<MovementSystem>();

            if (!movementSystem->isAnimating(entity))
            {
                movementSystem->moveItemToPlayer(entity);
            }
        }
    }
}

void PickupSystem::applyChest()
{
    std::unordered_map<std::string, int> upgradeList =
        SystemManager::getInstance()->getSystem<LevelSystem>()->upgradeGenerator(false);

    auto gameScene = dynamic_cast<GameScene*>(SystemManager::getInstance()->getCurrentScene());
    if (gameScene)
    {
        auto levelUpLayer = LevelUpOrChestEventLayer::create(false, upgradeList);  // isLevelUp = false
        if (levelUpLayer)
        {
            // Lấy vị trí của uiLayer
            ax::Vec2 uiLayerPos = gameScene->getUILayer()->getPosition();
            levelUpLayer->setPosition(uiLayerPos);
            gameScene->addChild(levelUpLayer, 1000);  // Thêm layer
            // gameScene->unscheduleUpdate();            // Dừng update
            SystemManager::getInstance()->setUpdateState(false);
        }
    }
}
