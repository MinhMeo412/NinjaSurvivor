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

    void upgradeWeapon(std::string weaponName);  // Nâng cấp vũ khí

    // Lấy danh sách entity vũ khí (moveSystem truy cập để xử lý vị trí)
    const std::vector<Entity>& getWeaponEntities() const { return weaponPool; }
    const std::vector<Entity>& getTempWeaponEntities() const { return tempWeaponPool; }

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

    using CreateFunc = std::function<Entity(std::string, bool)>;
    std::unordered_map<std::string, CreateFunc> createWeapon;

    using UpdateFunc = std::function<void(Entity, float)>;
    std::unordered_map<std::string, UpdateFunc> updateWeapon;

    using UpgradeFunc = std::function<void(std::string, int)>;
    std::unordered_map<std::string, UpgradeFunc> upgradeWeaponzxcv;

    Entity playerEntity = 0;             // Entity của player để gắn vị trí vũ khí
    std::vector<Entity> weaponPool;      // Pool chứa các entity vũ khí tái sử dụng
    std::vector<Entity> tempWeaponPool;  // Pool chứa các entity vũ khí không tái sử dụng

    // Tạo entity weapon
    Entity createSword(std::string weaponName, bool alreadyHave);
    Entity createShuriken(std::string weaponName, bool alreadyHave);
    Entity createKunai(std::string weaponName, bool alreadyHave);

    // Update weapon mỗi frame
    void updateSword(Entity weapon, float dt);
    void updateShuriken(Entity weapon, float dt);
    void updateKunai(Entity weapon, float dt);
    Entity kunaiEntity;

    // Tạo các tempEntityWeapon
    Entity createTempKunai(std::string weaponName);

    // Upgrade weapon

    // Khởi tạo vũ khí cho player
    void initializePlayerWeapon(Entity player);
};


#endif  // !__WEAPON_SYSTEM_H__
