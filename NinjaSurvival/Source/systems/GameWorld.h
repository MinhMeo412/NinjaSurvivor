#ifndef __GAME_WORLD_H__
#define __GAME_WORLD_H__

#include "entities/EntityManager.h"
#include "components/ComponentManager.h"

class GameWorld
{
public:
    GameWorld();
    EntityManager& getEntityManager() { return entityManager; }
    ComponentManager<IdentityComponent>& getIdentityManager() { return identityMgr; }
    ComponentManager<TransformComponent>& getTransformManager() { return transformMgr; }
    ComponentManager<SpriteComponent>& getSpriteManager() { return spriteMgr; }
    ComponentManager<AnimationComponent>& getAnimationManager() { return animationMgr; }
    ComponentManager<VelocityComponent>& getVelocityManager() { return velocityMgr; }
    ComponentManager<HitboxComponent>& getHitboxManager() { return hitboxMgr; }

    // Thêm các get cho các ComponentManager khác khi cần

private:
    EntityManager entityManager;
    ComponentManager<IdentityComponent> identityMgr;
    ComponentManager<TransformComponent> transformMgr;
    ComponentManager<SpriteComponent> spriteMgr;
    ComponentManager<AnimationComponent> animationMgr;
    ComponentManager<VelocityComponent> velocityMgr;
    ComponentManager<HitboxComponent> hitboxMgr;
    // Thêm các ComponentManager khác

};


#endif  // __GAME_WORLD_H__
