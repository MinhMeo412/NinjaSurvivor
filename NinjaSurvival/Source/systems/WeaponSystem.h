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

    void upgradeWeaponAndBuff(std::string name);  // Nâng cấp vũ khí
    std::vector<std::string> getPlayerInventory(bool isWp);

    // Lấy danh sách entity vũ khí (moveSystem truy cập để xử lý vị trí)
    const std::vector<Entity>& getWeaponEntities() const { return weaponPool; }
    const std::vector<Entity>& getTempWeaponEntities() const { return tempWeaponPool; }

    // Tạo enemy projectile
    Entity createEnemyProjectile(std::string weaponName, Entity attacker);

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
    std::unordered_map<std::string, UpgradeFunc> upgradeWeapon;
    std::unordered_map<std::string, UpgradeFunc> upgradeBuff;

    Entity playerEntity = 0;     
    std::vector<Entity> weaponPool;      // Pool chứa các entity vũ khí tái sử dụng
    std::vector<Entity> tempWeaponPool;  // Pool chứa các entity vũ khí không tái sử dụng

    // Tạo entity weapon
    Entity createSword(std::string weaponName, bool alreadyHave);
    std::vector<Entity> swordList;
    Entity createShuriken(std::string weaponName, bool alreadyHave);
    std::vector<Entity> shurikenList;
    Entity createKunai(std::string weaponName, bool alreadyHave);
    Entity kunaiEntity;
    Entity createTempKunai(std::string weaponName); // Tạo các tempEntityWeapon Kunai
    Entity createBigKunai(std::string weaponName, bool alreadyHave);
    std::vector<Entity> bigKunaiList;
    Entity createExplosionKunai(std::string weaponName, bool alreadyHave);
    std::vector<Entity> explosionKunaiList;
    Entity createSpinner(std::string weaponName, bool alreadyHave);
    std::vector<Entity> spinnerList;
    //Entity createLightningScroll(std::string weaponName, bool alreadyHave);
    //std::vector<Entity> lightningScrollList;
    Entity createNinjutsuSpell(std::string weaponName, bool alreadyHave);
    std::vector<Entity> ninjutsuSpellList;

    
    // Update weapon mỗi frame
    void updateEnemyProjectile(Entity weapon, float dt);

    void updateSword(Entity weapon, float dt);
    void updateShuriken(Entity weapon, float dt);
    void updateKunai(Entity weapon, float dt);
    void updateBigKunai(Entity weapon, float dt);
    void updateSpinner(Entity weapon, float dt);
    void updateExplosionKunai(Entity weapon, float dt);
    void updateNinjutsuSpell(Entity weapon, float dt);

    

    // Upgrade weapon
    void upgradeSword(std::string weaponName, int level);
    void upgradeShuriken(std::string weaponName, int level);
    void upgradeKunai(std::string weaponName, int level);
    void upgradeBigKunai(std::string weaponName, int level);
    void upgradeSpinner(std::string weaponName, int level);
    void upgradeExplosionKunai(std::string weaponName, int level);
    void upgradeNinjutsuSpell(std::string weaponName, int level);

    // Upgrade buff
    void upgradeAttack(std::string buffName, int level);
    void upgradeHealth(std::string buffName, int level);
    void upgradeSpeed(std::string buffName, int level);
    void upgradeXPGain(std::string buffName, int level);
    void upgradePickupRange(std::string buffName, int level);
    void upgradeReduceReceiveDamage(std::string buffName, int level);
    void upgradeCoinGain(std::string buffName, int level);
    void upgradeCurse(std::string buffName, int level);

    // Khởi tạo vũ khí cho player
    void initializePlayerWeapon(Entity player);
};


#endif  // !__WEAPON_SYSTEM_H__
