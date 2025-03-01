#ifndef __COMPONENT_MANAGER_H__
#define __COMPONENT_MANAGER_H__

#include <unordered_map>
#include <memory>
#include <typeindex>
#include "entities/Entity.h"
#include "Components.h"

template <typename T>
class ComponentManager
{
public:
    ComponentManager() = default;
    void addComponent(Entity entity, const T& component);
    T* getComponent(Entity entity);
    void removeComponent(Entity entity);

private:
    std::unordered_map<Entity, std::unique_ptr<T>> components;
};

#endif  // __COMPONENT_MANAGER_H__
