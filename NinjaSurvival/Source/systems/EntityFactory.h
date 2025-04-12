#ifndef __ENTITY_FACTORY_H__
#define __ENTITY_FACTORY_H__

#include "entities/Entity.h"
#include "components/ComponentManager.h"
#include "entities/EntityManager.h"
#include "axmol.h"

class EntityFactory
{
public:
    EntityFactory(EntityManager& em,
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
    Entity createEntity(const std::string& type, const std::string& name);

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

};

#endif  // __ENTITY_FACTORY_H__
