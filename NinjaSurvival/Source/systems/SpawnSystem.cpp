#include "SpawnSystem.h"

#include "SystemManager.h"
#include "GameData.h"
#include "EntityFactory.h"

SpawnSystem::SpawnSystem(EntityManager& em,
                           ComponentManager<TransformComponent>& tm,
                           ComponentManager<SpriteComponent>& sm,
                           ComponentManager<AnimationComponent>& am,
                           ComponentManager<VelocityComponent>& vm)
    : entityManager(em), transformMgr(tm), spriteMgr(sm), animationMgr(am), velocityMgr(vm) {}

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

    EntityFactory factory(entityManager, transformMgr, spriteMgr, animationMgr, velocityMgr);
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
}

void SpawnSystem::update(float dt)
{
    spawnTimer += dt;
    if (spawnTimer >= 3.0f)  // Sinh enemy mỗi 2 giây
    {
        for (int i = 0; i < 10; i++)
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

            EntityFactory factory(entityManager, transformMgr, spriteMgr, animationMgr, velocityMgr);
            enemyEntity = factory.createEntity("player", characterName);

            float x = rand() % 1000;
            float y = rand() % 1000;

            auto position = transformMgr.getComponent(enemyEntity);
            position->x   = x;
            position->y   = y;

            AXLOG("SpawnSystem: Created Enemy %s with ID %u", characterName.c_str(), enemyEntity);

            if (auto animationComp = animationMgr.getComponent(enemyEntity))
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
            if (auto spriteComp = spriteMgr.getComponent(enemyEntity))
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

        
        }
        spawnTimer = 0.0f;
    }
    
}

Entity SpawnSystem::getPlayerEntity() const
{
    return playerEntity;
}

Entity SpawnSystem::getEnemyEntity() const
{
    return enemyEntity;
}
