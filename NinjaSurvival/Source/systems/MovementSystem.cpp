#include "MovementSystem.h"

#include "JoystickSystem.h"
#include "SpawnSystem.h"
#include "SystemManager.h"

MovementSystem::MovementSystem(EntityManager& em,
                               ComponentManager<TransformComponent>& tm,
                               ComponentManager<VelocityComponent>& vm,
                               ComponentManager<AnimationComponent>& am)
    : entityManager(em), transformMgr(tm), velocityMgr(vm), animationMgr(am) {}

void MovementSystem::init() {}

void MovementSystem::update(float dt)
{
    auto systemManager  = SystemManager::getInstance();
    auto joystickSystem = systemManager->getSystem<JoystickSystem>();
    auto spawnSystem    = systemManager->getSystem<SpawnSystem>();
    if (!spawnSystem)
    {
        AXLOG("spawnSystem null");
        return;
    }

    Entity playerEntity = spawnSystem->getPlayerEntity();
    //Entity enemyEntity = spawnSystem->getEnemyEntity();
    auto entities       = entityManager.getActiveEntities();

    for (Entity entity : entities)
    {
        if (auto transform = transformMgr.getComponent(entity))
        {
            if (auto velocity = velocityMgr.getComponent(entity))
            {
                if (entity == playerEntity && joystickSystem)
                {
                    VelocityComponent joystickVelocity = joystickSystem->getVelocity();
                    velocity->vx                       = joystickVelocity.vx * 100.0f; //Sử dụng speed
                    velocity->vy                       = joystickVelocity.vy * 100.0f;

                    if (auto animation = animationMgr.getComponent(entity))
                    {
                        if (velocity->vx > 0)
                            animation->currentState = "moveRight"; //"moveRight";
                        else if (velocity->vx < 0)
                            animation->currentState = "moveLeft"; //"moveLeft";
                        else if (velocity->vy > 0)
                            animation->currentState = "moveDown";//Move up changes
                        else if (velocity->vy < 0)
                            animation->currentState = "moveDown";//"moveDown";
                        else
                            animation->currentState = "idle";
                    }
                }
                else
                {
                    auto playerPos = transformMgr.getComponent(playerEntity);
                    auto enemyPos  = transform;

                    ax::Vec2 direction = ax::Vec2 ( (playerPos->x - enemyPos->x) , (playerPos->y - enemyPos->y) );

                    if (!direction.isZero())
                    {
                        direction.normalize();
                    }

                    // Calculate the new position by moving in that direction
                    ax::Vec2 newPos = direction * 50.0f;

                    velocity->vx                       = newPos.x;  
                    velocity->vy                       = newPos.y;


                    if (auto animation = animationMgr.getComponent(entity))
                    {
                        if (velocity->vx > 0)
                            animation->currentState = "moveRight";  //"moveRight";
                        else if (velocity->vx < 0)
                            animation->currentState = "moveLeft";  //"moveLeft";
                        else if (velocity->vy > 0)
                            animation->currentState = "moveDown";  // Move up changes
                        else if (velocity->vy < 0)
                            animation->currentState = "moveDown";  //"moveDown";
                        else
                            animation->currentState = "idle";
                    }

                    //AXLOG("Not found any else entity");
                }

                //Di chuyển
                transform->x += velocity->vx * dt;
                transform->y += velocity->vy * dt;
            }
        }
    }
}
