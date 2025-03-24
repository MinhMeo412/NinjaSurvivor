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
                 ComponentManager<CooldownComponent>& cm);

    void init() override;
    void update(float dt) override;

    // Hàm xử lý sát thương khi va chạm xảy ra (gọi với callback từ collision system)
    void handleCollision(Entity e1, Entity e2);
    float getPlayerMaxHealth() const;
    float getPlayerCurrentHealth() const;

private:
    EntityManager& entityManager;
    ComponentManager<IdentityComponent>& identityMgr;
    ComponentManager<HealthComponent>& healthMgr;
    ComponentManager<AttackComponent>& attackMgr;
    ComponentManager<CooldownComponent>& cooldownMgr;

    // Công thức tính dame
    float calculateDamage(const AttackComponent* attack) const;

    // Trừ máu
    void applyDamage(Entity target, float damage);

    // Kiểm tra và cập nhật cooldown
    bool canDealDamage(Entity attacker);
    void resetCooldown(Entity attacker);

    // Callback khi player hết máu
    std::function<void()> onPlayerOutOfHealth;  
};

#endif  // __HEALTH_SYSTEM_H__
