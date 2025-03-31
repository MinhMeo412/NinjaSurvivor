#include "ItemSystem.h"

#include "SystemManager.h"
#include "GameData.h"
#include "EntityFactory.h"
#include "MapSystem.h"
#include "TimeSystem.h"
#include "CollisionSystem.h"


void ItemSystem::init()
{
    // Khởi tạo entity factory
    factory = std::make_unique<EntityFactory>(entityManager, identityMgr, transformMgr, spriteMgr, animationMgr,
                                              velocityMgr, hitboxMgr, healthMgr, attackMgr, cooldownMgr, speedMgr);

    for (int i = 0; i < 1000; i++)
        {
            Entity item = factory->createEntity("item", "coin");

            if (auto spriteComp = spriteMgr.getComponent(item))
            {
                spriteComp->initializeSprite();
            }

            auto transform = transformMgr.getComponent(item);
            if (!transform)
                continue;

            // Tính toán vị trí dựa trên số thứ tự i
            float offsetX = (i % 50) * 20.0f;  // Mỗi hàng có 50 coin
            float offsetY = (i / 50) * 20.0f;  // Xuống hàng sau mỗi 50 coin

            transform->x = 1000 + offsetX;
            transform->y = 1000 + offsetY;
        }
}


void ItemSystem::update(float dt)
{
    float elapsedTime = SystemManager::getInstance()->getSystem<TimeSystem>()->getElapsedTime();

    // Spawn item ngẫu nhiên mỗi x giây
    if (static_cast<int>(elapsedTime) % 30 == 0 && elapsedTime > 0)
    {
        //spawnRandomItems();
    }
}

// Spawn item khi quái chết
void ItemSystem::spawnItemAtDeath(Entity deadEntity, bool isBoss)
{
    auto transform = transformMgr.getComponent(deadEntity);
    if (!transform)
        return;

    ax::Vec2 position(transform->x, transform->y);

    if (isBoss)
    {
        // Boss: Nhiều XP gem, Coin, và 1 Chest
        spawnMultipleItems(position, "xp_gem", 3, 6, 0.8f);  // 3-6 XP gem, 80%
        spawnMultipleItems(position, "coin", 2, 5, 0.7f);    // 2-5 Coin, 70%
        spawnSingleItem(position, "chest", 1.0f);            // 100% Chest
    }
    else
    {
        // Enemy thường: XP gem, Coin, Heart
        spawnSingleItem(position, "xp_gem", 0.9f);  // 90%
        spawnSingleItem(position, "coin", 0.05f);    // 5%
        spawnSingleItem(position, "heart", 0.01f);   // 1%
    }
}

// Spawn một item với tỷ lệ
void ItemSystem::spawnSingleItem(const ax::Vec2& position, const std::string& name, float chance)
{
    if (static_cast<float>(rand()) / RAND_MAX < chance)
    {
        Entity item = factory->createEntity("item", name);

        if (auto spriteComp = spriteMgr.getComponent(item))
        {
            spriteComp->initializeSprite();
        }

        auto transform = transformMgr.getComponent(item);
        transform->x = position.x;
        transform->y = position.y;
    }
}

// Spawn nhiều item với số lượng ngẫu nhiên
void ItemSystem::spawnMultipleItems(const ax::Vec2& position,
                                    const std::string& name,
                                    int minCount,
                                    int maxCount,
                                    float chance)
{
    if (static_cast<float>(rand()) / RAND_MAX < chance)
    {
        int count = minCount + rand() % (maxCount - minCount + 1);
        for (int i = 0; i < count; ++i)
        {
            Entity item = factory->createEntity("item", name);

            if (auto spriteComp = spriteMgr.getComponent(item))
            {
                spriteComp->initializeSprite();
            }

            // Offset ngẫu nhiên để không chồng nhau
            float offsetX = (rand() % 21 - 10) * 1.0f;  // -10 đến 10
            float offsetY = (rand() % 21 - 10) * 1.0f;
            auto transform = transformMgr.getComponent(item);
            transform->x   = position.x + offsetX;
            transform->y   = position.y + offsetY;
        }
    }
}

// Spawn item ngẫu nhiên trên map
void ItemSystem::spawnRandomItems()
{
    int itemsToSpawn = rand() % 5 + 1;  // 1-3 item mỗi lần
    for (int i = 0; i < itemsToSpawn; ++i)
    {
        float chance = static_cast<float>(rand()) / RAND_MAX;
        std::string name;
        if (chance < 0.3f)
            name = "bomb";  // 30%
        else if (chance < 0.6f)
            name = "magnet";  // 30%
        else
            name = "health";  // 40%

        Entity item       = factory->createEntity("item", name);

        if (auto spriteComp = spriteMgr.getComponent(item))
        {
            spriteComp->initializeSprite();
        }

        setRandomMapPosition(item);
    }
}

bool ItemSystem::isSpawnOnCollisionTile(Entity entity, const ax::Vec2& spawnPosition)
{
    auto collisionSystem = SystemManager::getInstance()->getSystem<CollisionSystem>();
    return collisionSystem->isCollidingWithTileMap(entity, spawnPosition);
}

// Lấy vị trí ngẫu nhiên trong map
void ItemSystem::setRandomMapPosition(Entity entity)
{
    auto mapSystem  = SystemManager::getInstance()->getSystem<MapSystem>();
    float mapWidth  = mapSystem->getGridSize().x * mapSystem->getChunkSize().x;
    float mapHeight = mapSystem->getGridSize().y * mapSystem->getChunkSize().y;

    ax::Vec2 spawnPos(rand() % static_cast<int>(mapWidth), rand() % static_cast<int>(mapHeight));
    if (!isSpawnOnCollisionTile(entity, spawnPos))
    {
        auto transform = transformMgr.getComponent(entity);
        transform->x   = spawnPos.x;
        transform->y   = spawnPos.y;
    }
    //Random không được thì khỏi spawn
    entityManager.destroyEntity(entity);
}
