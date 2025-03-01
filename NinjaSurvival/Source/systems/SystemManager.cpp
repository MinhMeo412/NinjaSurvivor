#include "SystemManager.h"

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

void SystemManager::initSystems(Scene* scene)
{
    currentScene = scene;
    for (auto& system : systems)
    {
        system->init();
    }
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
