#ifndef __WEAPON_SYSTEM_H__
#define __WEAPON_SYSTEM_H__

#include "System.h"
#include "components/ComponentManager.h"
#include "entities/EntityManager.h"
#include "EntityFactory.h"
#include "axmol.h"

class WeaponSystem : public System
{
public:
    WeaponSystem(EntityManager& em,
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

    void upgradeWeapon();  // Nâng cấp vũ khí

    // Lấy danh sách entity vũ khí (moveSystem truy cập để xử lý vị trí)
    const std::vector<Entity>& getWeaponEntities() const { return weaponPool; }

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

    Entity playerEntity = 0;          // Entity của player để gắn vị trí vũ khí
    std::vector<Entity> weaponPool;   // Pool chứa các entity vũ khí

    Entity createSwordEntity(std::string weaponName);  // Tạo entity weapon

    // Khởi tạo vũ khí cho player
    void initializePlayerWeapon(Entity player);
};


#endif  // !__WEAPON_SYSTEM_H__
