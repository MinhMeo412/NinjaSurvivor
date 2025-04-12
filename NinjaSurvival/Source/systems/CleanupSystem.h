#ifndef __CLEANUP_SYSTEM_H__
#define __CLEANUP_SYSTEM_H__

#include "System.h"
#include "components/ComponentManager.h"
#include "entities/EntityManager.h"
#include "axmol.h"

class CleanupSystem : public System
{
public:
    CleanupSystem(EntityManager& em,
                  ComponentManager<SpriteComponent>& sm,
        ComponentManager<HitboxComponent>& hm)
        : entityManager(em), spriteMgr(sm), hitboxMgr(hm)
    {}

    void destroyEntity(Entity entity)
    {
        auto sprite = spriteMgr.getComponent(entity);
        if (sprite && sprite->gameSceneFrame)
        {
            // Đánh dấu entity inactive
            entityManager.destroyEntity(entity);
            // Fade out rồi xóa khỏi scene
            sprite->gameSceneFrame->runAction(ax::Sequence::create(ax::FadeOut::create(0.3f),
                                                                   ax::CallFunc::create([=]() {
                sprite->gameSceneFrame->removeFromParent();
                sprite->gameSceneFrame = nullptr;

                // Xóa component sau khi sprite đã biến mất
                spriteMgr.removeComponent(entity);

            }), nullptr));

            return;
        }
    }

    void destroyItem(Entity entity)
    {
        entityManager.destroyEntity(entity);
        auto sprite = spriteMgr.getComponent(entity);
        if (sprite && sprite->gameSceneFrame)
        {
            sprite->gameSceneFrame->removeFromParent();
            sprite->gameSceneFrame = nullptr;

            // Xóa component sau khi sprite đã biến mất
            spriteMgr.removeComponent(entity);
        }
        if (auto hitbox = hitboxMgr.getComponent(entity))
        {
            hitboxMgr.removeComponent(entity);
        }
    }

    void init() override {}

    void update(float dt) override
    {
        // Không cần update thường xuyên, chỉ xử lý khi được gọi
    }

private:
    EntityManager& entityManager;
    ComponentManager<SpriteComponent>& spriteMgr;
    ComponentManager<HitboxComponent>& hitboxMgr;
};

#endif  // __CLEANUP_SYSTEM_H__
