#ifndef __SPAWN_SYSTEM_H__
#define __SPAWN_SYSTEM_H__

#include "System.h"
#include "components/ComponentManager.h"
#include "entities/EntityManager.h"
#include "axmol.h"

class SpawnSystem : public System
{
public:
    SpawnSystem(EntityManager& em,
                 ComponentManager<TransformComponent>& tm,
                 ComponentManager<SpriteComponent>& sm,
                 ComponentManager<AnimationComponent>& am,
                 ComponentManager<VelocityComponent>& vm); 

    void init() override;
    void update(float dt) override;

    Entity getPlayerEntity() const;

private:
    EntityManager& entityManager;
    ComponentManager<TransformComponent>& transformMgr;
    ComponentManager<SpriteComponent>& spriteMgr;
    ComponentManager<AnimationComponent>& animationMgr;
    ComponentManager<VelocityComponent>& velocityMgr;

    Entity playerEntity = 0;
    float spawnTimer    = 0.0f;
};




#endif  // __SPAWN_SYSTEM_H__
