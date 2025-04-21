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
                                              velocityMgr, hitboxMgr, healthMgr, attackMgr, cooldownMgr, speedMgr,
                                              weaponInventoryMgr, durationMgr);

    int num = 1000;
    for (int i = 0; i < num; i++) 
    {
        Entity item = factory->createEntity("item", "chest");
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
        transform->x  = 1000 + offsetX;
        transform->y  = 1000 + offsetY;
    }

    for (int i = 0; i < num; i++)
    {
          Entity item = factory->createEntity("item", "greenGem");
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
          transform->y = 1500 + offsetY;
    }

    // Entity item = factory->createEntity("item", "magnet");
    // if (auto spriteComp = spriteMgr.getComponent(item))
    //{
    //     spriteComp->initializeSprite();
    // }
    // auto transform = transformMgr.getComponent(item);
    // transform->x  = 900 ;
    // transform->y  = 1600 ;
}

void ItemSystem::update(float dt)
{
    float elapsedTime = SystemManager::getInstance()->getSystem<TimeSystem>()->getElapsedTime();

    if (static_cast<int>(elapsedTime) - lastSpawnTime >= 30)
    {
        spawnRandomItems();
        lastSpawnTime = static_cast<int>(elapsedTime);
    }
}

// Spawn item khi quái chết
void ItemSystem::spawnItemAtDeath(Entity deadEntity, bool isBoss)
{
    float elapsedTime = SystemManager::getInstance()->getSystem<TimeSystem>()->getElapsedTime();
    auto transform    = transformMgr.getComponent(deadEntity);
    if (!transform)
        return;

    ax::Vec2 position(transform->x, transform->y);

    float greenGemRatio = 1.0f;
    float blueGemRatio  = 0.0f;
    float redGemRatio   = 0.0f;

    if (elapsedTime > 720)
    {
        greenGemRatio = 0.3f;
        blueGemRatio  = 0.5f;
        redGemRatio   = 0.2f;
    }
    else if (elapsedTime > 480)
    {
        greenGemRatio = 0.5f;
        blueGemRatio  = 0.4f;
        redGemRatio   = 0.1f;
    }
    else if (elapsedTime > 300)
    {
        greenGemRatio = 0.6f;
        blueGemRatio  = 0.3f;
        redGemRatio   = 0.1f;
    }
    else if (elapsedTime > 180)
    {
        greenGemRatio = 0.8f;
        blueGemRatio  = 0.2f;
        redGemRatio   = 0.0f;
    }
    else if (elapsedTime > 60)
    {
        greenGemRatio = 0.9f;
        blueGemRatio  = 0.1f;
        redGemRatio   = 0.0f;
    }

    float randVal = dist(rng);
    if (isBoss)
    {
        if (1)  // 100% rớt gem
        {
            if (randVal < greenGemRatio)
                spawnMultipleItems(position, "greenGem", 10, 20, 1.0f);
            else if (randVal < greenGemRatio + blueGemRatio)
                spawnMultipleItems(position, "blueGem", 10, 20, 1.0f);
            else
                spawnMultipleItems(position, "redGem", 10, 20, 1.0f);
        }

        spawnMultipleItems(position, "coin", 10, 20, 1.0f);
        spawnMultipleItems(position, "heart", 1, 3, 1.0f);
        spawnSingleItem(position, "chest", 1.0f);  // 100% Chest
    }
    else
    {
        float dropChance = dist(rng);
        if (dropChance < 0.8f)  // 80% rớt gem
        {
            if (randVal < greenGemRatio)
                spawnSingleItem(position, "greenGem", 1.0f);
            else if (randVal < greenGemRatio + blueGemRatio)
                spawnSingleItem(position, "blueGem", 1.0f);
            else
                spawnSingleItem(position, "redGem", 1.0f);
        }

        spawnSingleItem(position, "coin", 0.15f);   // 15% coin
        spawnSingleItem(position, "heart", 0.01f);  // 1% heart
        spawnSingleItem(position, "bomb", 0.005f);  // 0.5% bomb
    }
}

// Spawn một item với tỷ lệ
void ItemSystem::spawnSingleItem(const ax::Vec2& position, const std::string& name, float chance)
{
    if (dist(rng) < chance)
    {
        Entity item = factory->createEntity("item", name);

        if (auto spriteComp = spriteMgr.getComponent(item))
        {
            spriteComp->initializeSprite();
        }

        auto transform = transformMgr.getComponent(item);
        transform->x   = position.x;
        transform->y   = position.y;
    }
}

// Spawn nhiều item với số lượng ngẫu nhiên
void ItemSystem::spawnMultipleItems(const ax::Vec2& position,
                                    const std::string& name,
                                    int minCount,
                                    int maxCount,
                                    float chance)
{
    if (dist(rng) < chance)
    {
        int count = minCount + rand() % (maxCount - minCount + 1);
        for (int i = 0; i < count; ++i)
        {
            Entity item = factory->createEntity("item", name);

            if (auto spriteComp = spriteMgr.getComponent(item))
            {
                spriteComp->initializeSprite();
            }

            // Offset ngẫu nhiên trong vùng 64x64
            float offsetX  = (rand() % 65 - 32) * 1.0f;  // -32 đến 32
            float offsetY  = (rand() % 65 - 32) * 1.0f;
            auto transform = transformMgr.getComponent(item);
            transform->x   = position.x + offsetX;
            transform->y   = position.y + offsetY;
        }
    }
}

// Spawn item ngẫu nhiên trên map
void ItemSystem::spawnRandomItems()
{
    int itemsToSpawn = rand() % 10 + 1;  // 1-10 item mỗi lần
    for (int i = 0; i < itemsToSpawn; ++i)
    {
        float chance = dist(rng);
        std::string name;
        if (chance < 0.2f)
            name = "bomb";  // 20%
        else if (chance < 0.5f)
            name = "magnet";  // 30%
        else
            name = "heart";  // 50%

        Entity item = factory->createEntity("item", name);

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
    // Random không được thì khỏi spawn
    entityManager.destroyEntity(entity);
}
