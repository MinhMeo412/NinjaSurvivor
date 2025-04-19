#include "RenderSystem.h"
#include "SpawnSystem.h"
#include "CleanupSystem.h"


#include "SystemManager.h"

RenderSystem::RenderSystem(EntityManager& em,
                           ComponentManager<IdentityComponent>& im,
                           ComponentManager<SpriteComponent>& sm,
                           ComponentManager<TransformComponent>& tm,
                           ComponentManager<AnimationComponent>& am,
                           ComponentManager<HitboxComponent>& hm,
                           ComponentManager<CooldownComponent>& cdm,
                           ComponentManager<VelocityComponent>& vm,
                           ComponentManager<DurationComponent>& drm)
    : entityManager(em)
    , identityMgr(im)
    , spriteMgr(sm)
    , transformMgr(tm)
    , animationMgr(am)
    , hitboxMgr(hm)
    , cooldownMgr(cdm)
    , velocityMgr(vm)
    , durationMgr(drm)
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
    itemBatchNode = ax::SpriteBatchNode::create("Entity/Items/items.png");
    weaponBatchNode = ax::SpriteBatchNode::create("Entity/Weapons/weapon.png");
    numberBatchNode = ax::SpriteBatchNode::create("number.png");

    scene->addChild(enemyBatchNode, 3);
    //scene->addChild(bossBatchNode, 4);
    scene->addChild(itemBatchNode, 2); 
    scene->addChild(weaponBatchNode, 5);
    scene->addChild(numberBatchNode, 6);

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
            scene->addChild(sprite->gameSceneFrame, 4);
        }
    }
    else if (identity->type == "enemy")
    {
        sprite->setBatchNode(enemyBatchNode);
    }
    else if (identity->type == "boss")
    {
        //sprite->setBatchNode(bossBatchNode);
        if (sprite->gameSceneFrame->getParent() != scene)
        {
            scene->addChild(sprite->gameSceneFrame, 4);
        }
    }
    else if (identity->type == "item")
    {
        sprite->setBatchNode(itemBatchNode);
    }
    else if (identity->type == "weapon_melee")
    {
        sprite->setBatchNode(weaponBatchNode);
    }
    else if (identity->type == "weapon_projectile")
    {
        sprite->setBatchNode(weaponBatchNode);
    }
    else if (identity->type == "enemy_projectile")
    {
        sprite->setBatchNode(weaponBatchNode);
    }


    auto transform = transformMgr.getComponent(entity);
    if (transform)
    {
        sprite->gameSceneFrame->setPosition(transform->x, transform->y);
    }
}

void RenderSystem::update(float dt)
{
    auto start = std::chrono::high_resolution_clock::now();
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

        if (identityMgr.getComponent(entity)->name == "energy_ball")
        {
            updateEnemyProjectileSprite(entity);
            continue;
        }

        if (identityMgr.getComponent(entity)->name == "sword")
        {  // Không tự động update sword
            continue;
        }

        if (identityMgr.getComponent(entity)->name == "shuriken")
        {
            updateShurikenEntitySprite(entity);
            continue;
        }

        if (identityMgr.getComponent(entity)->name == "kunai")
        {
            updateKunaiEntitySprite(entity);
            continue;
        }

        if (identityMgr.getComponent(entity)->name == "big_kunai")
        {
            updateBigKunaiEntitySprite(entity);
            continue;
        }



        updateEntitySprite(entity, dt);
    }




    //updateDebugDraw(); //Bỏ nếu k vẽ viền nữa

    auto end      = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    //AXLOG("Thời gian thực thi RenderSystem: %ld ms", duration);
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
                    //AXLOG("Started animation %s with tag %d for entity %u", anim.c_str(), tag, entity);
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



void RenderSystem::onEntityDestroyed(Entity entity)
{
    auto sprite = spriteMgr.getComponent(entity);
    if (sprite && sprite->gameSceneFrame)
    {
        sprite->gameSceneFrame->removeFromParent();
        sprite->gameSceneFrame = nullptr;
    }
}


// Hàm set batch node cho sprite (chỉ dùng cho number)
void RenderSystem::setSpriteBatchNodeForSprite(ax::Sprite* sprite, const std::string& type)
{
    if (!sprite)
    {
        AXLOG("Error: Sprite is null in setSpriteBatchNodeForSprite");
        return;
    }
    if (type == "number" || type == "hit_effect")
    {
        numberBatchNode->addChild(sprite);
    }
}

// Hình ảnh enemy projectile (kiểu Conditional)
void RenderSystem::updateEnemyProjectileSprite(Entity entity)
{
    auto sprite    = spriteMgr.getComponent(entity);
    auto transform = transformMgr.getComponent(entity);
    auto duration  = durationMgr.getComponent(entity);

    sprite->gameSceneFrame->setPosition(transform->x, transform->y);
    sprite->gameSceneFrame->setScale(transform->scale);

    if (duration->duration <= 0)
    {
        SystemManager::getInstance()->getSystem<CleanupSystem>()->destroyItem(entity);
    }
}

// Hình ảnh weapon sword (kiểu OneShot)
void RenderSystem::updateSwordEntitySprite(Entity entity)
{
    auto sprite    = spriteMgr.getComponent(entity);
    auto transform = transformMgr.getComponent(entity);
    auto vel       = velocityMgr.getComponent(entity);
    // Lật sprite dựa trên hướng vận tốc
    if (vel->vx > 0)  // Sang phải
    {
        sprite->gameSceneFrame->setScaleX(1.0f * transform->scale);  // Không lật
    }
    else if (vel->vx < 0)  // Sang trái
    {
        sprite->gameSceneFrame->setScaleX(-1.0f * transform->scale);  // Lật ngang
    }
    sprite->gameSceneFrame->setScaleY(transform->scale);
    sprite->gameSceneFrame->setPosition(transform->x, transform->y);
    sprite->gameSceneFrame->setOpacity(255);
    sprite->gameSceneFrame->runAction(ax::FadeOut::create(0.5f));
}

// Hình ảnh weapon shuriken (kiểu Conditional)
void RenderSystem::updateShurikenEntitySprite(Entity entity)
{
    auto sprite    = spriteMgr.getComponent(entity);
    auto transform = transformMgr.getComponent(entity);
    auto cooldown  = cooldownMgr.getComponent(entity);

    sprite->gameSceneFrame->setScale(transform->scale);
    sprite->gameSceneFrame->setPosition(transform->x, transform->y);

    // Định nghĩa tag cho hành động xoay
    const int ROTATE_ACTION_TAG = 1001;

    // Kiểm tra cooldown để quyết định hiển thị hay ẩn
    if (cooldown->cooldownTimer <= cooldown->cooldownDuration)
    {
        sprite->gameSceneFrame->setOpacity(0);
        sprite->gameSceneFrame->stopActionByTag(ROTATE_ACTION_TAG);  // Chỉ dừng hành động xoay
    }
    else
    {
        sprite->gameSceneFrame->setOpacity(255);

        // Kiểm tra xem hành động xoay đã chạy chưa
        if (!sprite->gameSceneFrame->getActionByTag(ROTATE_ACTION_TAG))
        {
            auto rotateAction = ax::RepeatForever::create(ax::RotateBy::create(0.5f, 360));
            rotateAction->setTag(ROTATE_ACTION_TAG);
            sprite->gameSceneFrame->runAction(rotateAction);
        }
    }
}

// Hình ảnh weapon kunai (kiểu Conditional)
void RenderSystem::updateKunaiEntitySprite(Entity entity)
{
    auto sprite    = spriteMgr.getComponent(entity);
    auto transform = transformMgr.getComponent(entity);
    auto cooldown  = cooldownMgr.getComponent(entity);
    auto vel       = velocityMgr.getComponent(entity);

    sprite->gameSceneFrame->setPosition(transform->x, transform->y);

    auto angleRadians = std::atan2(vel->vy, -vel->vx);
    // Chuyển sang độ và điều chỉnh
    float angleDegrees = angleRadians * 180.0f / M_PI - 90.0f;

    // Đảm bảo góc nằm trong khoảng [0, 360)
    if (angleDegrees >= 360.0f)
    {
        angleDegrees -= 360.0f;
    }
    else if (angleDegrees < 0.0f)
    {
        angleDegrees += 360.0f;
    }
    sprite->gameSceneFrame->setRotation(angleDegrees);

    if (cooldown->cooldownTimer <= 0)
    {
        SystemManager::getInstance()->getSystem<CleanupSystem>()->destroyItem(entity);
    }
}

void RenderSystem::updateBigKunaiEntitySprite(Entity entity)
{
    auto sprite    = spriteMgr.getComponent(entity);
    auto transform = transformMgr.getComponent(entity);
    auto cooldown  = cooldownMgr.getComponent(entity);
    auto vel       = velocityMgr.getComponent(entity);

    sprite->gameSceneFrame->setScale(transform->scale);
    sprite->gameSceneFrame->setPosition(transform->x, transform->y);

    auto angleRadians = std::atan2(vel->vy, -vel->vx);
    // Chuyển sang độ và điều chỉnh
    float angleDegrees = angleRadians * 180.0f / M_PI - 90.0f;

    // Đảm bảo góc nằm trong khoảng [0, 360)
    if (angleDegrees >= 360.0f)
    {
        angleDegrees -= 360.0f;
    }
    else if (angleDegrees < 0.0f)
    {
        angleDegrees += 360.0f;
    }
    sprite->gameSceneFrame->setRotation(angleDegrees);

    if (cooldown->cooldownTimer > cooldown->cooldownDuration)
    {
        sprite->gameSceneFrame->setOpacity(255);
    }
    else if (cooldown->cooldownTimer <= cooldown->cooldownDuration)
    {
        sprite->gameSceneFrame->setOpacity(0);
    }
}
