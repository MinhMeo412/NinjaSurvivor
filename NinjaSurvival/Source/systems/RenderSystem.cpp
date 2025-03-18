#include "RenderSystem.h"
#include "SpawnSystem.h"


#include "SystemManager.h"

RenderSystem::RenderSystem(EntityManager& em,
                           ComponentManager<IdentityComponent>& im,
                           ComponentManager<SpriteComponent>& sm,
                           ComponentManager<TransformComponent>& tm,
                           ComponentManager<AnimationComponent>& am,
                           ComponentManager<HitboxComponent>& hm)
    : entityManager(em), identityMgr(im), spriteMgr(sm), transformMgr(tm), animationMgr(am), hitboxMgr(hm)
{}

void RenderSystem::init()
{
    scene = SystemManager::getInstance()->getCurrentScene();
    if (!scene)
    {
        AXLOG("Error: No scene available for RenderSystem");
        return;
    }

    auto spawnSystem = SystemManager::getInstance()->getSystem<SpawnSystem>();
    if (!spawnSystem)
    {
        AXLOG("Error: SpawnSystem not found in RenderSystem::init");
        return;
    }

    // Khởi tạo sprite cho player
    Entity playerEntity = spawnSystem->getPlayerEntity();
    initializeEntitySprite(playerEntity);
}

void RenderSystem::initializeEntitySprite(Entity entity)
{
    if (auto sprite = spriteMgr.getComponent(entity))
    {
        if (!sprite->gameSceneFrame)
        {//Kiểm tra gameSceneFrame tồn tại
            AXLOG("Error: No gameSceneFrame for entity %u in init", entity);
            return;
        }

        // Thêm sprite vào scene
        if (sprite->gameSceneFrame->getParent() != scene)
        {
            scene->addChild(sprite->gameSceneFrame, 3);  // zOrder = 3 cho entity
            AXLOG("Added sprite for entity %u to scene", entity);
        }

        // Đặt vị trí ban đầu
        if (auto transform = transformMgr.getComponent(entity))
        {
            sprite->gameSceneFrame->setPosition(transform->x, transform->y);
        }
        // Chạy animation mặc định (idle) nếu có
        //if (auto animation = animationMgr.getComponent(entity))
        //{
        //    animation->currentState = "idle";   // Trạng thái mặc định
        //    std::string entityName  = "Ninja";  // TODO: Lấy tên động từ config hoặc SpriteComponent
        //    std::string anim        = entityName + animation->currentState;
        //    auto cachedAnim         = ax::AnimationCache::getInstance()->getAnimation(anim);
        //    if (cachedAnim)
        //    {
        //        auto animate = ax::Animate::create(cachedAnim);
        //        if (animate)
        //        {
        //            animation->currentAction = ax::RepeatForever::create(animate);
        //            animation->currentAction->setTag(AnimationComponent::ACTION_TAG_IDLE);
        //            sprite->gameSceneFrame->runAction(animation->currentAction);
        //            AXLOG("Initialized animation %s for entity %u", anim.c_str(), entity);
        //        }
        //    }
        //}
    }
}

void RenderSystem::update(float dt)
{
    if (!scene)
    {
        AXLOG("Error: No scene available for RenderSystem");
        return;
    }

    auto entities = entityManager.getActiveEntities();
    for (Entity entity : entities)
    {
        if (auto sprite = spriteMgr.getComponent(entity))
        {
            if (!sprite->gameSceneFrame)
            {
                AXLOG("Warning: No gameSceneFrame for entity %u", entity);
                continue;
            }
                
            if (auto animation = animationMgr.getComponent(entity))
            {
                if (!animation->currentState.empty())
                {
                    std::string entityName;
                    if (auto identity = identityMgr.getComponent(entity))
                    {
                        entityName = identity->name;
                    }
                    std::string anim = entityName + animation->currentState;

                    int tag = 0;

                    // Gán tag dựa trên currentState
                    if (animation->currentState == "moveLeft")
                        tag = AnimationComponent::ACTION_TAG_LEFT;
                    else if (animation->currentState == "moveRight")
                        tag = AnimationComponent::ACTION_TAG_RIGHT;
                    else if (animation->currentState == "moveDown")
                        tag = AnimationComponent::ACTION_TAG_DOWN;
                    else if (animation->currentState == "idle")
                        tag = AnimationComponent::ACTION_TAG_IDLE;

                    if (tag != 0 && (animation->currentAction == nullptr || animation->currentAction->getTag() != tag))
                    {
                        auto cachedAnim = ax::AnimationCache::getInstance()->getAnimation(anim);
                        if (cachedAnim)
                        {
                            auto animate = ax::Animate::create(cachedAnim);
                            if (animate)
                            {
                                if (animation->currentAction)
                                {
                                    sprite->gameSceneFrame->stopAction(animation->currentAction);
                                }
                                animation->currentAction = ax::RepeatForever::create(animate);
                                animation->currentAction->setTag(tag);
                                sprite->gameSceneFrame->runAction(animation->currentAction);
                                AXLOG("Started animation %s with tag %d for entity %u", anim.c_str(), tag, entity);
                            }
                            else
                            {
                                AXLOG("Warning: Failed to create Animate for %s, entity %u", anim.c_str(), entity);
                            }
                        }
                        else
                        {
                            AXLOG("Warning: Animation %s not found in cache for entity %u", anim.c_str(), entity);
                        }
                    }
                }
            }


            //Vị trí
            if (auto transform = transformMgr.getComponent(entity))
            {
                sprite->gameSceneFrame->setPosition(transform->x, transform->y);
                //sprite->gameSceneFrame->setScale(transform->scale);
            }
            else
            {
                AXLOG("Warning: No transform component for entity %u", entity);
            }

            if (sprite->gameSceneFrame->getParent() != scene)
            {
                scene->addChild(sprite->gameSceneFrame, 3);  // Hoặc thêm zOrder vào transform
                AXLOG("Added sprite for entity %u to scene", entity);
            }

            //Vẽ hitbox
            if (auto hitbox = hitboxMgr.getComponent(entity))
            {
                if (!sprite->debugDrawNode)
                {
                    sprite->debugDrawNode = ax::DrawNode::create();
                    scene->addChild(sprite->debugDrawNode, 10);
                }

                sprite->debugDrawNode->clear();  // Xóa hình cũ
                ax::Vec2 hitboxPos = sprite->gameSceneFrame->getPosition();
                sprite->debugDrawNode->drawRect(hitboxPos - ax::Vec2(hitbox->size.width / 2, hitbox->size.height / 2),
                                                hitboxPos + ax::Vec2(hitbox->size.width / 2, hitbox->size.height / 2),
                                                ax::Color4F::RED);
            }
        }
    }
}


