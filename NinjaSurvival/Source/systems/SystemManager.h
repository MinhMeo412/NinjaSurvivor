#ifndef __SYSTEM_MANAGER_H__
#define __SYSTEM_MANAGER_H__

#include "System.h"
#include "axmol.h"
#include "GameWorld.h"

class SystemManager
{
public:
    static SystemManager* getInstance();

    void initSystems(ax::Scene* scene, GameWorld* world, ax::Layer* uiLayer);
    void update(float dt);

    void resetSystems();                     

    template <typename T>
    T* getSystem() const
    {
        for (const auto& system : systems)
        {
            if (auto casted = dynamic_cast<T*>(system.get()))
            {
                return casted;
            }
        }
        return nullptr;
    }

    ax::Scene* getCurrentScene() const { return currentScene; }
    ax::Layer* getSceneLayer() const { return sceneLayer; }

    void setUpdateState(bool update) { canUpdate = update; }

private:
    SystemManager() = default;
    static SystemManager* instance;

    std::vector<std::unique_ptr<System>> systems;
    ax::Scene* currentScene = nullptr;
    ax::Layer* sceneLayer   = nullptr;
    GameWorld* gameWorld = nullptr;

    bool canUpdate = true;

    void addSystem(std::unique_ptr<System> system);
    void registerSystem(const std::string& systemType);
};

#endif  // __SYSTEM_MANAGER_H__
