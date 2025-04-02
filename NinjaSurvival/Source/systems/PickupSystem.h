#ifndef __PICKUP_SYSTEM_H__
#define __PICKUP_SYSTEM_H__

#include "System.h"
#include "components/ComponentManager.h"
#include "entities/EntityManager.h"
#include "axmol.h"

class PickupSystem : public System
{
public:
    PickupSystem(EntityManager& em, ComponentManager<IdentityComponent>& im, ComponentManager<TransformComponent>& tm);
    void init() override;
    void update(float dt) override;
    void applyPickupEffect(std::string& itemName);

private:
    EntityManager& entityManager;
    ComponentManager<IdentityComponent>& identityMgr;
    ComponentManager<TransformComponent>& transformMgr;

    std::unordered_set<std::string> pickupItems;
    std::unordered_set<std::string> touchItems;

    float PICKUP_RANGE = 0.0f;
    float TOUCH_RANGE  = 0.0f;

    void applyHeart();
    void applyCoin();
    void applyXPGem(std::string& itemName);
    void applyBomb();
    void applyMagnet();
    void applyChest();
};

#endif  // __PICKUP_SYSTEM_H__
