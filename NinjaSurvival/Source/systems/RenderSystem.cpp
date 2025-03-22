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

    enemyBatchNode      = ax::SpriteBatchNode::create("Entity/Enemy/enemy.png");
    //bossBatchNode       = ax::SpriteBatchNode::create("Entity/Boss/boss.png");
    //itemWeaponBatchNode = ax::SpriteBatchNode::create("Entity/ItemWeapon/item.png");

    scene->addChild(enemyBatchNode, 3);
    //scene->addChild(bossBatchNode, 4);
    //scene->addChild(itemWeaponBatchNode, 5); //Có lẽ nên chia riêng item với weapon

    debugDrawNode = ax::DrawNode::create();
    scene->addChild(debugDrawNode, 10);

    // Khởi tạo sprite cho player
    Entity playerEntity = spawnSystem->getPlayerEntity();
    addSpriteToScene(playerEntity);
}

void RenderSystem::addSpriteToScene(Entity entity)
{
    auto sprite = spriteMgr.getComponent(entity);
    if (!sprite || !sprite->gameSceneFrame)
    {
        AXLOG("Warning: Sprite or gameSceneFrame is null for entity %u", entity);
        return;
    }

    auto identity = identityMgr.getComponent(entity);
    if (!identity)
    {
        AXLOG("Warning: Identity is null for entity %u", entity);
        return;
    }

    if (identity->type == "player")
    {
        if (sprite->gameSceneFrame->getParent() != scene)
        {
            scene->addChild(sprite->gameSceneFrame, 3);
        }
    }
    else if (identity->type == "enemy")
    {
        sprite->setBatchNode(enemyBatchNode);
    }
    //else if (identity->type == "boss")
    //{
    //    sprite->setBatchNode(bossBatchNode);
    //}
    //else if (identity->type == "item" || identity->type == "weapon")
    //{
    //    sprite->setBatchNode(itemWeaponBatchNode);
    //}

    TransformComponent* transform = transformMgr.getComponent(entity);
    if (transform)
    {
        sprite->gameSceneFrame->setPosition(transform->x, transform->y);
    }
}

void RenderSystem::update(float dt)
{
    for (auto entity : entityManager.getActiveEntities())
    {
        auto sprite = spriteMgr.getComponent(entity);
        if (!sprite)
        {
            AXLOG("Warning: No sprite component for entity %u", entity);
            continue;
        }

        if (!sprite->gameSceneFrame || !sprite->gameSceneFrame->getParent())
        {
            addSpriteToScene(entity);
            continue;
        }

        updateEntitySprite(entity, dt);
    }
    updateDebugDraw(); //Bỏ nếu k vẽ viền nữa
}

void RenderSystem::updateEntitySprite(Entity entity, float dt)
{
    auto sprite         = spriteMgr.getComponent(entity);
    auto transform      = transformMgr.getComponent(entity);
    auto animation      = animationMgr.getComponent(entity);

    if (!sprite || !sprite->gameSceneFrame || !transform)
        return;

    sprite->gameSceneFrame->setPosition(transform->x, transform->y);
    sprite->gameSceneFrame->setScale(transform->scale);

    if (animation && !animation->currentState.empty())
    {
        std::string entityName;
        if (auto identity = identityMgr.getComponent(entity))
        {
            entityName = identity->name;
        }
        std::string anim = entityName + animation->currentState;
        int tag          = 0;

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

void RenderSystem::updateDebugDraw()
{
    debugDrawNode->clear();
    for (auto entity : entityManager.getActiveEntities())
    {
        SpriteComponent* sprite = spriteMgr.getComponent(entity);
        HitboxComponent* hitbox = hitboxMgr.getComponent(entity);
        if (sprite && sprite->gameSceneFrame && hitbox)
        {
            ax::Vec2 pos = sprite->gameSceneFrame->getPosition();
            debugDrawNode->drawRect(pos - ax::Vec2(hitbox->size.width / 2, hitbox->size.height / 2),
                                    pos + ax::Vec2(hitbox->size.width / 2, hitbox->size.height / 2), ax::Color4F::RED);
        }
    }
}



