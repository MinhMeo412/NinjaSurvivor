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
                 ComponentManager<SpriteComponent>& sm,
                 ComponentManager<TransformComponent>& tm,
                 ComponentManager<AnimationComponent>& am);

    void init() override;
    void update(float dt) override;

private:
    EntityManager& entityManager;
    ComponentManager<SpriteComponent>& spriteMgr;
    ComponentManager<TransformComponent>& transformMgr;
    ComponentManager<AnimationComponent>& animationMgr;
};




#endif  // __RENDER_SYSTEM_H__
