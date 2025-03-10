#include "RenderSystem.h"

#include "SystemManager.h"

RenderSystem::RenderSystem(EntityManager& em,
                           ComponentManager<SpriteComponent>& sm,
                           ComponentManager<TransformComponent>& tm,
                           ComponentManager<AnimationComponent>& am)
    : entityManager(em), spriteMgr(sm), transformMgr(tm), animationMgr(am) {}

void RenderSystem::init()
{
}

void RenderSystem::update(float dt)
{
    auto scene = SystemManager::getInstance()->getCurrentScene();
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
                    std::string anim = std::string("Ninja") + animation->currentState; //Tên entity (sẽ kiếm chỗ để load tên entity thay vì trực tiếp) và state
                    AXLOG("%s", anim.c_str());
                    int tag = 0;

                    // Gán tag dựa trên currentState
                    if (animation->currentState == "moveLeft")
                        tag = AnimationComponent::ACTION_TAG_LEFT;
                    else if (animation->currentState == "moveRight")
                        tag = AnimationComponent::ACTION_TAG_RIGHT;
                    else if (animation->currentState == "moveUp")
                        tag = AnimationComponent::ACTION_TAG_UP;
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
                //sprite->frame->setScale(transform->scale);
                sprite->gameSceneFrame->setScale(3); //Đang có lỗi khi setScale
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
        }
    }
}
