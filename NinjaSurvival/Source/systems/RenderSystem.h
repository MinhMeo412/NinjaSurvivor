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
                 ComponentManager<HitboxComponent>& hm,
                 ComponentManager<CooldownComponent>& cdm,
                 ComponentManager<VelocityComponent>& vm,
                 ComponentManager<DurationComponent>& drm);

    void init() override;
    void update(float dt) override;

    void onEntityDestroyed(Entity entity);
    void updateEnemyProjectileSprite(Entity entity);
    void updateSwordEntitySprite(Entity entity);
    void updateShurikenEntitySprite(Entity entity);
    void updateKunaiEntitySprite(Entity entity);
    void updateBigKunaiEntitySprite(Entity entity);


    // Hàm để DamageTextSystem gọi
    void setSpriteBatchNodeForSprite(ax::Sprite* sprite, const std::string& type);

private:
    EntityManager& entityManager;
    ComponentManager<IdentityComponent>& identityMgr;
    ComponentManager<SpriteComponent>& spriteMgr;
    ComponentManager<TransformComponent>& transformMgr;
    ComponentManager<AnimationComponent>& animationMgr;
    ComponentManager<HitboxComponent>& hitboxMgr;
    ComponentManager<CooldownComponent>& cooldownMgr;
    ComponentManager<VelocityComponent>& velocityMgr;
    ComponentManager<DurationComponent>& durationMgr;
    

    ax::Scene* scene                        = nullptr;
    ax::SpriteBatchNode* enemyBatchNode     = nullptr;
    //ax::SpriteBatchNode* bossBatchNode      = nullptr;
    ax::SpriteBatchNode* itemBatchNode      = nullptr;
    ax::SpriteBatchNode* weaponBatchNode    = nullptr;
    ax::SpriteBatchNode* numberBatchNode    = nullptr;

    ax::DrawNode* debugDrawNode              = nullptr;

    void addSpriteToScene(Entity entity);
    void updateEntitySprite(Entity entity, float dt);
    
    void updateDebugDraw();
};




#endif  // __RENDER_SYSTEM_H__
