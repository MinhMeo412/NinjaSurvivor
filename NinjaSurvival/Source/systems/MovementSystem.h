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
                   ComponentManager<AnimationComponent>& am,
                   ComponentManager<HitboxComponent>& hm);

    void update(float dt) override;
    void init() override;

private:
    EntityManager& entityManager;
    ComponentManager<TransformComponent>& transformMgr;
    ComponentManager<VelocityComponent>& velocityMgr;
    ComponentManager<AnimationComponent>& animationMgr;
    ComponentManager<HitboxComponent>& hitboxMgr;

    ax::TMXTiledMap* getChunkTileMap(const ax::Vec2& position);  // Lấy chunk từ MapSystem

    ax::Vec2 resolveCollision(Entity entity, const ax::Vec2& currentPos, const ax::Vec2& moveStep);
    bool isCollidingWithTileMap(Entity entity, const ax::Vec2& position);
    ax::Vec2 convertToTileCoord(const ax::Vec2& position, ax::TMXTiledMap* tiledMap);
};

#endif  // __MOVEMENT_SYSTEM_H__
