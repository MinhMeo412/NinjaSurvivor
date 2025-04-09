#ifndef __SPAWN_SYSTEM_H__
#define __SPAWN_SYSTEM_H__

#include "System.h"
#include "components/ComponentManager.h"
#include "entities/EntityManager.h"
#include "EntityFactory.h"
#include "axmol.h"

class SpawnSystem : public System
{
public:
    SpawnSystem(EntityManager& em,
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
                             ComponentManager<WeaponInventoryComponent>& wim)
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
    {}
    
    void init() override;
    void update(float dt) override;

    Entity getPlayerEntity() const { return playerEntity; }

    // Callback khi enemy chết
    std::function<void()> onEnemyDeath;

    // Lấy vị trí world của player
    ax::Vec2 getPlayerPosition() const;

    //Re-position cho moveSystem
    ax::Vec2 getRandomSpawnPosition(Entity entity, const ax::Vec2& playerPosition);

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

    std::unique_ptr<EntityFactory> factory;

    Entity playerEntity = 0;
    float spawnTimer    = 0.0f;
    float spawnInterval  = 2.0f;   // Spawn mỗi 2 giây
    int livingEnemyCount = 0;      // Đếm số enemy đang hoạt động
    bool isBossActive    = false;  

    void spawnEnemies(float elapsedTime);
    void spawnBoss(float elapsedTime);
    void adjustEnemySpawnDuringBoss(bool isBossActive);

    Entity spawnEntity(const std::string& type, const std::string& name, const ax::Vec2& playerPosition);
    

    bool isSpawnOnCollisionTile(Entity entity, const ax::Vec2& spawnPosition);
};




#endif  // __SPAWN_SYSTEM_H__
