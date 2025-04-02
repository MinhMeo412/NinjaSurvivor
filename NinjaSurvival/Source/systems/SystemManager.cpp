#include "SystemManager.h"

#include "MapSystem.h"
#include "JoystickSystem.h"
#include "TimeSystem.h"
#include "SpawnSystem.h"
#include "MovementSystem.h"
#include "RenderSystem.h"
#include "CameraSystem.h"
#include "CollisionSystem.h"
#include "HealthSystem.h"
#include "ItemSystem.h"
#include "PickupSystem.h"
#include "LevelSystem.h"
#include "CleanupSystem.h"

using namespace ax;

SystemManager* SystemManager::instance = nullptr;

SystemManager* SystemManager::getInstance()
{
    if (!instance)
    {
        instance = new SystemManager();
    }
    return instance;
}

void SystemManager::initSystems(ax::Scene* scene, GameWorld* world, ax::Layer* uiLayer)
{
    currentScene = scene;
    gameWorld    = world;
    sceneLayer   = uiLayer;

    if (!gameWorld)
    {
        AXLOG("Error: GameWorld is null in SystemManager");
        return;
    }

    if (!currentScene)
    {
        AXLOG("Error: Scene is null in SystemManager");
        return;
    }

    registerSystem("MapSystem");
    registerSystem("JoystickSystem");
    registerSystem("TimeSystem");
    registerSystem("SpawnSystem");
    registerSystem("CameraSystem");

    registerSystem("MovementSystem");
    registerSystem("CollisionSystem");
    registerSystem("HealthSystem");
    registerSystem("ItemSystem");
    registerSystem("PickupSystem");
    registerSystem("LevelSystem");


    registerSystem("RenderSystem");
    registerSystem("CleanupSystem");


    for (auto& system : systems)
        system->init();
}

void SystemManager::update(float dt)
{
    auto start = std::chrono::high_resolution_clock::now();

    for (auto& system : systems)
    {
        system->update(dt);
    }

    auto end      = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    //AXLOG("Thời gian thực thi SystemManager: %ld ms", duration);
}

void SystemManager::resetSystems()
{
    systems.clear();
    currentScene = nullptr;
}

void SystemManager::addSystem(std::unique_ptr<System> system)
{
    systems.push_back(std::move(system));
}

void SystemManager::registerSystem(const std::string& systemType)
{
    if (!gameWorld)
        return;

    //Thêm JoystickSystem
    if (systemType == "JoystickSystem")
        addSystem(std::make_unique<JoystickSystem>());

    // Thêm MapSystem
    else if (systemType == "MapSystem")
        addSystem(std::make_unique<MapSystem>());

    // Thêm TimeSystem
    else if (systemType == "TimeSystem")
        addSystem(std::make_unique<TimeSystem>());

    // Thêm SpawnSystem
    else if (systemType == "SpawnSystem")
        addSystem(std::make_unique<SpawnSystem>(
            gameWorld->getEntityManager(), gameWorld->getIdentityManager(), gameWorld->getTransformManager(),
            gameWorld->getSpriteManager(), gameWorld->getAnimationManager(), gameWorld->getVelocityManager(),
            gameWorld->getHitboxManager(), gameWorld->getHealthManager(), gameWorld->getAttackManager(),
            gameWorld->getCooldownManager(), gameWorld->getSpeedManager()));

    // Thêm RenderSystem
    else if (systemType == "RenderSystem")
        addSystem(std::make_unique<RenderSystem>(gameWorld->getEntityManager(), gameWorld->getIdentityManager(),
                                                 gameWorld->getSpriteManager(), gameWorld->getTransformManager(),
                                                 gameWorld->getAnimationManager(), gameWorld->getHitboxManager()));

    // Thêm MovementSystem
    else if (systemType == "MovementSystem")
        addSystem(std::make_unique<MovementSystem>(gameWorld->getEntityManager(), gameWorld->getIdentityManager(),
                                                   gameWorld->getTransformManager(), gameWorld->getVelocityManager(),
                                                   gameWorld->getAnimationManager(), gameWorld->getHitboxManager(),
                                                   gameWorld->getSpeedManager()));

    // Thêm CameraSystem
    else if (systemType == "CameraSystem")
        addSystem(std::make_unique<CameraSystem>(gameWorld->getEntityManager(), gameWorld->getTransformManager()));

    // Thêm CollisionSystem
    else if (systemType == "CollisionSystem")
        addSystem(std::make_unique<CollisionSystem>(gameWorld->getEntityManager(), gameWorld->getIdentityManager(),
                                                    gameWorld->getTransformManager(), gameWorld->getHitboxManager()));

    // Thêm HealthSystem
    else if (systemType == "HealthSystem")
        addSystem(std::make_unique<HealthSystem>(gameWorld->getEntityManager(), gameWorld->getIdentityManager(),
                                                 gameWorld->getHealthManager(), gameWorld->getAttackManager(),
                                                 gameWorld->getCooldownManager()));

    // Thêm ItemSystem
    else if (systemType == "ItemSystem")
        addSystem(std::make_unique<ItemSystem>(
            gameWorld->getEntityManager(), gameWorld->getIdentityManager(), gameWorld->getTransformManager(),
            gameWorld->getSpriteManager(), gameWorld->getAnimationManager(), gameWorld->getVelocityManager(),
            gameWorld->getHitboxManager(), gameWorld->getHealthManager(), gameWorld->getAttackManager(),
            gameWorld->getCooldownManager(), gameWorld->getSpeedManager()));

    // Thêm PickupSystem
    else if (systemType == "PickupSystem")
        addSystem(std::make_unique<PickupSystem>(gameWorld->getEntityManager(), gameWorld->getIdentityManager(),
                                                 gameWorld->getTransformManager()));

    // Thêm CleanupSystem
    else if (systemType == "CleanupSystem")
        addSystem(std::make_unique<CleanupSystem>(gameWorld->getEntityManager(), gameWorld->getSpriteManager()));

    // Thêm LevelSystem
    else if (systemType == "LevelSystem")
        addSystem(std::make_unique<LevelSystem>());
}   
