#include "EntityManager.h"

EntityManager::EntityManager() : nextEntityId(0) {}

Entity EntityManager::createEntity()
{
    Entity entity    = nextEntityId++;//integer overflow issue

    //Create key and add Value
    entities[entity] = true;
    return entity;
}

void EntityManager::destroyEntity(Entity entity)
{
    if (entities.find(entity) != entities.end())
    {
        entities[entity] = false;
    }
}

bool EntityManager::isEntityActive(Entity entity) const
{
    auto it = entities.find(entity);
    return it != entities.end() && it->second;
}

std::vector<Entity> EntityManager::getActiveEntities() const
{   
    std::vector<Entity> activeEntities;
    for (const auto& [entity, active] : entities)
    {
        if (active)
        {
            activeEntities.push_back(entity);
        }
    }
    return activeEntities;
}
