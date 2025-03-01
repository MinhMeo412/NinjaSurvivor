#ifndef __ENTITY_MANAGER_H__
#define __ENTITY_MANAGER_H__

#include <unordered_map>
#include <vector>
#include "Entity.h"

class EntityManager
{
public:
    EntityManager();
    Entity createEntity();
    void destroyEntity(Entity entity);
    bool isEntityActive(Entity entity) const;
    std::vector<Entity> getActiveEntities() const;

private:
    unsigned int nextEntityId;
    std::unordered_map<Entity, bool> entities;
};

#endif  // __ENTITY_MANAGER_H__
