#ifndef __GAME_WORLD_H__
#define __GAME_WORLD_H__

#include "entities/EntityManager.h"
#include "components/ComponentManager.h"

class GameWorld
{
public:
    GameWorld();
    EntityManager& getEntityManager() { return entityManager; }
    ComponentManager<TransformComponent>& getTransformManager() { return transformMgr; }
    ComponentManager<SpriteComponent>& getSpriteManager() { return spriteMgr; }
    ComponentManager<AnimationComponent>& getAnimationManager() { return animationMgr; }
    ComponentManager<VelocityComponent>& getVelocityManager() { return velocityMgr; }
    // Thêm các get cho các ComponentManager khác khi cần

private:
    EntityManager entityManager;
    ComponentManager<TransformComponent> transformMgr;
    ComponentManager<SpriteComponent> spriteMgr;
    ComponentManager<AnimationComponent> animationMgr;
    ComponentManager<VelocityComponent> velocityMgr;
    // Thêm các ComponentManager khác

};


#endif  // __GAME_WORLD_H__
