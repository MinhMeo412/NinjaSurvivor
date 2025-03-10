#include "SystemManager.h"

#include "MapSystem.h"
#include "JoystickSystem.h"
#include "SpawnSystem.h"
#include "MovementSystem.h"
#include "RenderSystem.h"


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

void SystemManager::initSystems(ax::Scene* scene, GameWorld* world)
{
    currentScene = scene;
    gameWorld    = world;

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

    if (systemType == "JoystickSystem")
        addSystem(std::make_unique<JoystickSystem>());
    else if (systemType == "MapSystem")
        addSystem(std::make_unique<MapSystem>());
    else if (systemType == "SpawnSystem")
        addSystem(std::make_unique<SpawnSystem>(gameWorld->getEntityManager(), gameWorld->getTransformManager(),
                                                gameWorld->getSpriteManager(), gameWorld->getAnimationManager(),
                                                gameWorld->getVelocityManager()));
    else if (systemType == "RenderSystem")
        addSystem(std::make_unique<RenderSystem>(gameWorld->getEntityManager(), gameWorld->getSpriteManager(),
                                                 gameWorld->getTransformManager(), gameWorld->getAnimationManager()));
    else if (systemType == "MovementSystem")
        addSystem(std::make_unique<MovementSystem>(gameWorld->getEntityManager(), gameWorld->getTransformManager(),
                                                   gameWorld->getVelocityManager(), gameWorld->getAnimationManager()));
}
