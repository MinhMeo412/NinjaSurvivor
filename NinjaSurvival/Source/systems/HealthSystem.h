#ifndef __HEALTH_SYSTEM_H__
#define __HEALTH_SYSTEM_H__

#include "System.h"
#include "components/ComponentManager.h"
#include "entities/EntityManager.h"
#include "axmol.h"

class HealthSystem : public System
{
public:
    HealthSystem(EntityManager& em,
                 ComponentManager<IdentityComponent>& im,
                 ComponentManager<HealthComponent>& hm,
                 ComponentManager<AttackComponent>& am,
                 ComponentManager<CooldownComponent>& cm,
                 ComponentManager<DurationComponent>& drm);

    void init() override;
    void update(float dt) override;

    // Hàm xử lý sát thương khi va chạm xảy ra (gọi với callback từ collision system)
    void handleCollision(Entity e1, Entity e2);

    std::unordered_map<Entity, std::vector<Entity>> damagedEnemy;
    void handleWeaponCollision(std::unordered_map<Entity, std::vector<Entity>> damagedEnemy);

    float getPlayerMaxHealth() const;
    float getPlayerCurrentHealth() const;
    void setPlayerCurrentHealth(float hpRecover);

    void applyBombDamageToAll(float damage);

private:
    EntityManager& entityManager;
    ComponentManager<IdentityComponent>& identityMgr;
    ComponentManager<HealthComponent>& healthMgr;
    ComponentManager<AttackComponent>& attackMgr;
    ComponentManager<CooldownComponent>& cooldownMgr;
    ComponentManager<DurationComponent>& durationMgr;

    // Công thức tính dame
    float calculateEnemyDamage(const AttackComponent* attack);
    float calculatePlayerDamage(const AttackComponent* attack);

    // Trừ máu
    void applyDamage(Entity target, float damage);

    // Kiểm tra và cập nhật cooldown
    bool canWeaponDealDame(Entity weapon, Entity target);
    bool canDealDamage(Entity attacker);
    void resetCooldown(Entity attacker);

    // Callback khi player hết máu
    std::function<void()> onPlayerOutOfHealth;

    // Khởi tạo std::random_device và mt19937 với seed
    std::random_device rd;     // Lấy entropy từ hệ thống
    std::mt19937 gen;       // Tạo bộ sinh số pseudo-random
    // Hàm để sinh một số ngẫu nhiên thực (float) trong khoảng [min, max]
    float getRandomFloat(float min, float max)
    {
        std::uniform_real_distribution<float> dist(min, max);
        return dist(gen);
    }
};

#endif  // __HEALTH_SYSTEM_H__
