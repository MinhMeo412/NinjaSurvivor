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
                  ComponentManager<SpriteComponent>& sm)
        : entityManager(em), spriteMgr(sm)
    {}

    void destroyEntity(Entity entity)
    {
        // Xóa sprite khỏi scene
        auto sprite = spriteMgr.getComponent(entity);
        if (sprite && sprite->gameSceneFrame)
        {
            sprite->gameSceneFrame->removeFromParent();
            sprite->gameSceneFrame = nullptr;
        }

        // Xóa component
        spriteMgr.removeComponent(entity);

        // Đánh dấu entity inactive
        entityManager.destroyEntity(entity);
    }

    void init() override {}

    void update(float dt) override
    {
        // Không cần update thường xuyên, chỉ xử lý khi được gọi
    }

private:
    EntityManager& entityManager;
    ComponentManager<SpriteComponent>& spriteMgr;
};

#endif  // __CLEANUP_SYSTEM_H__
