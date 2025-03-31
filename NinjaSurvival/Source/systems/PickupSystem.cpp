#include "PickupSystem.h"
#include "SpawnSystem.h"
#include "MovementSystem.h"
#include "SystemManager.h"

PickupSystem::PickupSystem(EntityManager& em, ComponentManager<IdentityComponent>& im, ComponentManager<TransformComponent>& tm)
    : entityManager(em), identityMgr(im), transformMgr(tm)
{}

void PickupSystem::init()
{
    PICKUP_RANGE = 100.0f; //+buff
    TOUCH_RANGE  = 10.0f;
}

void PickupSystem::update(float dt)
{
    auto spawnSystem    = SystemManager::getInstance()->getSystem<SpawnSystem>();
    auto movementSystem = SystemManager::getInstance()->getSystem<MovementSystem>();
    if (!spawnSystem || !movementSystem)
        return;

    ax::Vec2 playerPos       = spawnSystem->getPlayerPosition();

    for (auto entity : entityManager.getActiveEntities())
    {
        auto identity = identityMgr.getComponent(entity);
        if (!identity || identity->type != "item")
            continue;

        auto transform = transformMgr.getComponent(entity);
        if (!transform)
            continue;

        ax::Vec2 itemPos(transform->x, transform->y);
        float distance = playerPos.distance(itemPos);

        bool shouldPickup = false;
        if (identity->name == "xp_gem" || identity->name == "coin" || identity->name == "heart")
        {
            shouldPickup = (distance <= PICKUP_RANGE);
        }
        else if (identity->name == "bomb" || identity->name == "magnet" || identity->name == "chest")
        {
            shouldPickup = (distance <= TOUCH_RANGE);
        }

        if (shouldPickup && !movementSystem->isAnimating(entity))
        {
            movementSystem->moveItemToPlayer(entity);
        }
    }
}

