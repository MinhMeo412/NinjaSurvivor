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
               ComponentManager<SpeedComponent>& spm,
               ComponentManager<WeaponInventoryComponent>& wim,
               ComponentManager<DurationComponent>& drm)
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
        , weaponInventoryMgr(wim)
        , durationMgr(drm)
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
    ComponentManager<WeaponInventoryComponent>& weaponInventoryMgr;
    ComponentManager<DurationComponent>& durationMgr;

    std::unique_ptr<EntityFactory> factory;
    void spawnSingleItem(const ax::Vec2& position, const std::string& name, float chance);
    void spawnMultipleItems(const ax::Vec2& position,
                            const std::string& name,
                            int minCount,
                            int maxCount,
                            float chance);
    void spawnRandomItems();
    int lastSpawnTime = 0;
    bool isSpawnOnCollisionTile(Entity entity, const ax::Vec2& spawnPosition);
    void setRandomMapPosition(Entity entity);

    std::mt19937 rng{std::random_device{}()};              
    std::uniform_real_distribution<float> dist{0.0f, 1.0f};

    //Xem xét thêm 1 loại item có thể nâng cấp chỉ số nhân vật vĩnh viễn
};




#endif  // __ITEM_SYSTEM_H__
