#include "SpawnSystem.h"

#include "SystemManager.h"
#include "GameData.h"
#include "EntityFactory.h"

SpawnSystem::SpawnSystem(EntityManager& em,
                           ComponentManager<IdentityComponent>& im,
                           ComponentManager<TransformComponent>& tm,
                           ComponentManager<SpriteComponent>& sm,
                           ComponentManager<AnimationComponent>& am,
                           ComponentManager<VelocityComponent>& vm,
                           ComponentManager<HitboxComponent>& hm)
    : entityManager(em)
    , identityMgr(im)
    , transformMgr(tm)
    , spriteMgr(sm)
    , animationMgr(am)
    , velocityMgr(vm)
    , hitboxMgr(hm)
{}

void SpawnSystem::init()
{
    auto scene = SystemManager::getInstance()->getCurrentScene();
    if (!scene)
    {
        AXLOG("Error: No scene available for PlayerSystem");
        return;
    }

    std::string characterName = GameData::getInstance()->getSelectedCharacter();
    if (characterName.empty())
    {
        AXLOG("Error: No character selected in GameData");
        return;
    }

    EntityFactory factory(entityManager, identityMgr, transformMgr, spriteMgr, animationMgr, velocityMgr, hitboxMgr);
    playerEntity = factory.createEntity("player", characterName);

    AXLOG("SpawnSystem: Created player %s with ID %u", characterName.c_str(), playerEntity);

    if (auto animationComp = animationMgr.getComponent(playerEntity))
    {
        animationComp->initializeAnimations();
        animationComp->currentState = "idle";

        AXLOG("Create animation");

        AXLOG("AnimationComponent Info:");
        AXLOG("plist: %s", animationComp->plist.c_str());
        AXLOG("frameDuration: %f", animationComp->frameDuration);
        AXLOG("currentState: %s", animationComp->currentState.c_str());

        AXLOG("Frames Map:");
        for (const auto& [state, frameList] : animationComp->frames)
        {
            std::string frameNames;
            for (const auto& frameName : frameList)
            {
                frameNames += frameName + " ";
            }
            AXLOG("  %s: [%s]", state.c_str(), frameNames.c_str());
        }
    }
    if (auto spriteComp = spriteMgr.getComponent(playerEntity))
    {
        spriteComp->initializeSprite();
        AXLOG("Create sprite");

        AXLOG("SpriteComponent Info:");
        AXLOG("Filename: %s", spriteComp->filename.c_str());

        if (spriteComp->frame)
        {
            AXLOG("Sprite is initialized.");
            AXLOG("Sprite position: (%.2f, %.2f)", spriteComp->frame->getPositionX(),
                  spriteComp->frame->getPositionY());
            AXLOG("Sprite scale: (%.2f, %.2f)", spriteComp->frame->getScaleX(), spriteComp->frame->getScaleY());
            AXLOG("Sprite visible: %s", spriteComp->frame->isVisible() ? "true" : "false");
            AXLOG("Sprite texture ID: %p", spriteComp->frame->getTexture());
        }
    }



    // Spawn một enemy "Slime"  để test va chạm
    Entity enemyEntity = factory.createEntity("enemy", "Slime");
    if (enemyEntity != 0)
    {
        if (auto transform = transformMgr.getComponent(enemyEntity))
        {
            transform->x = 800.0f;  // Vị trí cố định cho enemy
            transform->y = 1100.0f;
            AXLOG("SpawnSystem: Created enemy Slime with ID %u at (%f, %f)", enemyEntity, transform->x, transform->y);
        }
        if (auto animationComp = animationMgr.getComponent(enemyEntity))
        {
            animationComp->initializeAnimations();
            animationComp->currentState = "idle";
            AXLOG("Spawned enemy animation for entity %u", enemyEntity);
        }
        if (auto spriteComp = spriteMgr.getComponent(enemyEntity))
        {
            spriteComp->initializeSprite();
            AXLOG("Spawned enemy sprite for entity %u", enemyEntity);
        }
    }
}

void SpawnSystem::update(float dt)
{
    /* chưa dùng
    spawnTimer += dt;
    if (spawnTimer >= 2.0f)  // Sinh enemy mỗi 2 giây
    {
        EntityFactory factory(entityManager, identityMgr, transformMgr, spriteMgr, animationMgr, velocityMgr, hitboxMgr);
        Entity enemy = factory.createEntity("enemy", "Slime");
        if (enemy != 0)
        {
            if (auto transform = transformMgr.getComponent(enemy))
            {
                transform->x = 700.0f;
                transform->y = 1000.0f;
            }
            if (auto animationComp = animationMgr.getComponent(enemy))
            {
                animationComp->initializeAnimations();
                animationComp->currentState = "idle";

                AXLOG("Create animation");

                AXLOG("AnimationComponent Info:");
                AXLOG("plist: %s", animationComp->plist.c_str());
                AXLOG("frameDuration: %f", animationComp->frameDuration);
                AXLOG("currentState: %s", animationComp->currentState.c_str());

                AXLOG("Frames Map:");
                for (const auto& [state, frameList] : animationComp->frames)
                {
                    std::string frameNames;
                    for (const auto& frameName : frameList)
                    {
                        frameNames += frameName + " ";
                    }
                    AXLOG("  %s: [%s]", state.c_str(), frameNames.c_str());
                }
            }
            if (auto spriteComp = spriteMgr.getComponent(enemy))
            {
                spriteComp->initializeSprite();
                AXLOG("Create sprite");

                AXLOG("SpriteComponent Info:");
                AXLOG("Filename: %s", spriteComp->filename.c_str());

                if (spriteComp->frame)
                {
                    AXLOG("Sprite is initialized.");
                    AXLOG("Sprite position: (%.2f, %.2f)", spriteComp->frame->getPositionX(),
                          spriteComp->frame->getPositionY());
                    AXLOG("Sprite scale: (%.2f, %.2f)", spriteComp->frame->getScaleX(), spriteComp->frame->getScaleY());
                    AXLOG("Sprite visible: %s", spriteComp->frame->isVisible() ? "true" : "false");
                    AXLOG("Sprite texture ID: %p", spriteComp->frame->getTexture());
                }
            }
            spawnTimer = 0.0f;
        }
    }
    */
}

Entity SpawnSystem::getPlayerEntity() const
{
    return playerEntity;
}
