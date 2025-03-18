#ifndef __RENDER_SYSTEM_H__
#define __RENDER_SYSTEM_H__

#include "System.h"
#include "components/ComponentManager.h"
#include "entities/EntityManager.h"
#include "axmol.h"

class RenderSystem : public System
{
public:
    RenderSystem(EntityManager& em,
                 ComponentManager<IdentityComponent>& im,
                 ComponentManager<SpriteComponent>& sm,
                 ComponentManager<TransformComponent>& tm,
                 ComponentManager<AnimationComponent>& am,
                 ComponentManager<HitboxComponent>& hm);

    void init() override;
    void update(float dt) override;


private:
    EntityManager& entityManager;
    ComponentManager<IdentityComponent>& identityMgr;
    ComponentManager<SpriteComponent>& spriteMgr;
    ComponentManager<TransformComponent>& transformMgr;
    ComponentManager<AnimationComponent>& animationMgr;
    ComponentManager<HitboxComponent>& hitboxMgr;

    ax::Scene* scene = nullptr;

    void initializeEntitySprite(Entity entity);
};




#endif  // __RENDER_SYSTEM_H__
