#ifndef __ITEM_SYSTEM_H__
#define __ITEM_SYSTEM_H__

#include "System.h"
#include "components/ComponentManager.h"
#include "entities/EntityManager.h"
#include "EntityFactory.h"
#include "axmol.h"

class ItemSystem : public System
{
public:
    ItemSystem(EntityManager& em,
                ComponentManager<IdentityComponent>& im,
                ComponentManager<TransformComponent>& tm,
                ComponentManager<SpriteComponent>& sm,
                ComponentManager<AnimationComponent>& am,
                ComponentManager<VelocityComponent>& vm,
                ComponentManager<HitboxComponent>& hm,
                ComponentManager<HealthComponent>& hem,
                ComponentManager<AttackComponent>& atm,
                ComponentManager<CooldownComponent>& cdm,
                ComponentManager<SpeedComponent>& spm)
        : entityManager(em)
        , identityMgr(im)
        , transformMgr(tm)
        , spriteMgr(sm)
        , animationMgr(am)
        , velocityMgr(vm)
        , hitboxMgr(hm)
        , healthMgr(hem)
        , attackMgr(atm)
        , cooldownMgr(cdm)
        , speedMgr(spm)
    {}
    
    void init() override;
    void update(float dt) override;

    void spawnItemAtDeath(Entity deadEntity, bool isBoss);

private:
    EntityManager& entityManager;
    ComponentManager<IdentityComponent>& identityMgr;
    ComponentManager<TransformComponent>& transformMgr;
    ComponentManager<SpriteComponent>& spriteMgr;
    ComponentManager<AnimationComponent>& animationMgr;
    ComponentManager<VelocityComponent>& velocityMgr;
    ComponentManager<HitboxComponent>& hitboxMgr;
    ComponentManager<HealthComponent>& healthMgr;
    ComponentManager<AttackComponent>& attackMgr;
    ComponentManager<CooldownComponent>& cooldownMgr;
    ComponentManager<SpeedComponent>& speedMgr;

    std::unique_ptr<EntityFactory> factory;
    void spawnSingleItem(const ax::Vec2& position, const std::string& name, float chance);
    void spawnMultipleItems(const ax::Vec2& position,
                            const std::string& name,
                            int minCount,
                            int maxCount,
                            float chance);
    void spawnRandomItems();
    bool isSpawnOnCollisionTile(Entity entity, const ax::Vec2& spawnPosition);
    void setRandomMapPosition(Entity entity);
};




#endif  // __ITEM_SYSTEM_H__
