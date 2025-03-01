#ifndef __SYSTEM_MANAGER_H__
#define __SYSTEM_MANAGER_H__

#include "System.h"
#include "axmol.h"

class SystemManager
{
public:
    static SystemManager* getInstance();
    void initSystems(ax::Scene* scene);             
    void update(float dt);                            
    void resetSystems();                     
    void addSystem(std::unique_ptr<System> system); 


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

private:
    SystemManager() = default;
    static SystemManager* instance;             
    std::vector<std::unique_ptr<System>> systems;
    ax::Scene* currentScene = nullptr;            
};

#endif  // __SYSTEM_MANAGER_H__
