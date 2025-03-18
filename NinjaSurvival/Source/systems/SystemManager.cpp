#include "SystemManager.h"

#include "MapSystem.h"
#include "JoystickSystem.h"
#include "SpawnSystem.h"
#include "MovementSystem.h"
#include "RenderSystem.h"
#include "CameraSystem.h"


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
    
    registerSystem("SpawnSystem");
    registerSystem("CameraSystem");

    registerSystem("MovementSystem");
    registerSystem("RenderSystem");

    for (auto& system : systems)
        system->init();
}

void SystemManager::update(float dt)
{
    for (auto& system : systems)
    {
        system->update(dt);
    }
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

    //Thêm MapSystem
    else if (systemType == "MapSystem")
        addSystem(std::make_unique<MapSystem>());

    //Thêm SpawnSystem
    else if (systemType == "SpawnSystem")
        addSystem(std::make_unique<SpawnSystem>(gameWorld->getEntityManager(), gameWorld->getIdentityManager(),
                                                gameWorld->getTransformManager(),
                                                gameWorld->getSpriteManager(), gameWorld->getAnimationManager(),
                                                gameWorld->getVelocityManager(), gameWorld->getHitboxManager()));

    //Thêm RenderSystem
    else if (systemType == "RenderSystem")
        addSystem(std::make_unique<RenderSystem>(gameWorld->getEntityManager(), gameWorld->getIdentityManager(),
                                                 gameWorld->getSpriteManager(),
                                                 gameWorld->getTransformManager(), gameWorld->getAnimationManager(),
                                                 gameWorld->getHitboxManager()));

    //Thêm MovementSystem
    else if (systemType == "MovementSystem")
        addSystem(std::make_unique<MovementSystem>(gameWorld->getEntityManager(), gameWorld->getTransformManager(),
                                                   gameWorld->getVelocityManager(), gameWorld->getAnimationManager(),
                                                   gameWorld->getHitboxManager()));

    //Thêm CameraSystem
    else if (systemType == "CameraSystem")
        addSystem(std::make_unique<CameraSystem>(gameWorld->getEntityManager(), gameWorld->getTransformManager()));
}
