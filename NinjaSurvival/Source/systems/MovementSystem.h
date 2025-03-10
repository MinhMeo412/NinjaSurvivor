#ifndef __MOVEMENT_SYSTEM_H__
#define __MOVEMENT_SYSTEM_H__

#include "System.h"
#include "components/ComponentManager.h"
#include "entities/EntityManager.h"

class MovementSystem : public System
{
public:
    MovementSystem(EntityManager& em,
                   ComponentManager<TransformComponent>& tm,
                   ComponentManager<VelocityComponent>& vm,
                   ComponentManager<AnimationComponent>& am);

    void update(float dt) override;
    void init() override;

private:
    EntityManager& entityManager;
    ComponentManager<TransformComponent>& transformMgr;
    ComponentManager<VelocityComponent>& velocityMgr;
    ComponentManager<AnimationComponent>& animationMgr;
};

#endif  // __MOVEMENT_SYSTEM_H__
