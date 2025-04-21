#include "SpawnSystem.h"
#include "ShopSystem.h"
#include "Utils.h"
#include "SystemManager.h"
#include "GameData.h"
#include "EntityFactory.h"
#include "MapSystem.h"
#include "TimeSystem.h"
#include "CollisionSystem.h"
#include "MovementSystem.h"
#include "gameUI/GameSceneUILayer.h"

void SpawnSystem::init()
{
    spawnRate = 1 + ShopSystem::getInstance()->getShopBuff("SpawnRate");
    // Khởi tạo entity factory
    factory = std::make_unique<EntityFactory>(entityManager, identityMgr, transformMgr, spriteMgr, animationMgr,
                                              velocityMgr, hitboxMgr, healthMgr, attackMgr, cooldownMgr, speedMgr,
                                              weaponInventoryMgr, durationMgr);

    std::string characterName = GameData::getInstance()->getSelectedCharacter();
    if (characterName.empty())
    {
        AXLOG("Error: No character selected in GameData");
        return;
    }

    playerEntity = factory->createEntity("player", characterName);

    AXLOG("SpawnSystem: Created player %s with ID %u", characterName.c_str(), playerEntity);

    if (auto spriteComp = spriteMgr.getComponent(playerEntity))
    {
        spriteComp->initializeSprite();
    }

    if (auto animationComp = animationMgr.getComponent(playerEntity))
    {
        animationComp->initializeAnimations();
        animationComp->currentState = "idle";
    }

    // Thiết lập callback
    onEnemyDeath = [this]() {
        livingEnemyCount--;
        if (livingEnemyCount < 0)
            livingEnemyCount = 0;
    };

    // Test
    int gridSize = 2;
    int spacing  = 16;
    for (int i = 0; i < 4; i++)
    {
        Entity entity = factory->createEntity("enemy", "Octopus");
        ax::Vec2 spawnPos(800, 1000);
        if (auto transform = transformMgr.getComponent(entity))
        {
            int row      = i / gridSize;  // chỉ số hàng
            int col      = i % gridSize;  // chỉ số cột
            transform->x = spawnPos.x + (col * spacing);
            transform->y = spawnPos.y + (row * spacing);
        }
        if (auto spriteComp = spriteMgr.getComponent(entity))
        {
            spriteComp->initializeSprite();
        }
        if (auto animationComp = animationMgr.getComponent(entity))
        {
            animationComp->initializeAnimations();
            animationComp->currentState = "idle";
        }
    }

    // spawnBoss(10);
}

void SpawnSystem::update(float dt)
{
    spawnTimer += dt;

    // Lấy elapsedTime từ TimeSystem
    TimeSystem* timeSystem = SystemManager::getInstance()->getSystem<TimeSystem>();
    if (!timeSystem)
    {
        AXLOG("Error: TimeSystem not found in SpawnSystem::update");
        return;
    }
    float elapsedTime = timeSystem->getElapsedTime();

    // Spawn enemy mỗi spawnInterval = 2s
    // if (spawnTimer >= spawnInterval)
    //{
    //    spawnEnemies(elapsedTime);
    //    spawnBoss(elapsedTime);
    //    spawnTimer = 0.0f;  // Reset timer
    //}
}

void SpawnSystem::spawnEnemies(float elapsedTime)
{
    int numEnemies;  // Số enemy spawn
    int minute = elapsedTime / 60;
    numEnemies = (3 + minute) * spawnRate;

    // Giới hạn spawn
    if (livingEnemyCount < maxEnemies)
    {
        numEnemies = std::min(numEnemies, maxEnemies - livingEnemyCount);  // Chỉ spawn tối đa đủ số max
    }
    else if (livingEnemyCount >= maxEnemies)
    {
        numEnemies = 0;  // Không spawn nữa
    }

    // Điều chỉnh tỷ lệ spawn khi boss xuất hiện (hoặc cho = 0 để ko spawn)
    float spawnRateMultiplier = isBossActive ? 0.5f : 1.0f;
    numEnemies                = static_cast<int>(numEnemies * spawnRateMultiplier);

    // Tỷ lệ spawn từng loại quái
    float slimeRatio = 0.0, snakeRatio = 0.0, bearRatio = 0.0, octopusRatio = 0.0;
    if (elapsedTime < 60)  // 0-1 phút
    {
        slimeRatio = 1.0f;
    }
    else if (elapsedTime < 180)  // 1-3 phút
    {
        slimeRatio = 0.8f;
        snakeRatio = 0.1;
        bearRatio  = 0.1f;
    }
    else if (elapsedTime < 420)  // 3-7 phút
    {
        slimeRatio = 0.6f;
        snakeRatio = 0.2f;
        bearRatio  = 0.2f;
    }
    else if (elapsedTime < 600)  // 7-10 phút
    {
        slimeRatio   = 0.4f;
        snakeRatio   = 0.2f;
        bearRatio    = 0.3f;
        octopusRatio = 0.1f;
    }
    else if (elapsedTime < 840)  // 10-14 phút
    {
        slimeRatio   = 0.3f;
        snakeRatio   = 0.1f;
        bearRatio    = 0.45f;
        octopusRatio = 0.15f;
    }
    else  // >14 phút
    {
        snakeRatio   = 0.1f;
        bearRatio    = 0.7f;
        octopusRatio = 0.2f;
    }

    // Lấy vị trí player
    auto playerTransform = transformMgr.getComponent(playerEntity);
    if (!playerTransform)
        return;
    ax::Vec2 playerPos = ax::Vec2(playerTransform->x, playerTransform->y);

    for (int i = 0; i < numEnemies; i++)
    {
        float randVal = static_cast<float>(rand()) / RAND_MAX;  // Số ngẫu nhiên từ 0.0 - 1.0
        Entity enemy;

        if (randVal < slimeRatio)
        {
            enemy = spawnEntity("enemy", "Slime", playerPos);
        }
        else if (randVal < slimeRatio + snakeRatio)
        {
            enemy = spawnEntity("enemy", "Snake", playerPos);
        }
        else if (randVal < slimeRatio + snakeRatio + bearRatio)
        {
            enemy = spawnEntity("enemy", "Bear", playerPos);
        }
        else
        {
            enemy = spawnEntity("enemy", "Octopus", playerPos);
        }

        if (isBossActive)
        {
            if (auto health = healthMgr.getComponent(enemy))
            {
                health->maxHealth *= 3.0f;
                health->currentHealth = health->maxHealth;
            }
            if (auto name = identityMgr.getComponent(enemy)->name != "Octopus")
            {
                auto speed   = speedMgr.getComponent(enemy);
                speed->speed = std::min(speed->speed * 1.5f, 500.0f);
            }
        }

        livingEnemyCount++;  // Tăng số lượng enemy sống
    }
}

void SpawnSystem::spawnBoss(float elapsedTime)
{
    // if (static_cast<int>(elapsedTime) % 180 == 0 && elapsedTime > 0) // Mỗi 3 phút
    {
        auto playerTransform = transformMgr.getComponent(playerEntity);
        if (!playerTransform)
            return;
        ax::Vec2 playerPos = ax::Vec2(playerTransform->x, playerTransform->y);

        std::string bossName = nameList[Utils::getRandomInt(0, nameList.size() - 1)];
        Entity boss          = spawnEntity("boss", bossName, playerPos);

        bossSpawnCount++;
        if (boss)
        {
            auto health = healthMgr.getComponent(boss);
            auto speed  = speedMgr.getComponent(boss);
            auto attack = attackMgr.getComponent(boss);
            if (health)  // +50% hp mỗi lần spawn
            {
                health->maxHealth     = health->maxHealth * (1.0 + (0.5 * (bossSpawnCount - 1)));
                health->currentHealth = health->maxHealth;
            }
            if (speed)  // +20% speed mỗi lần spawn
            {
                float speedCalculate = speed->speed * (1.0 + (0.2 * (bossSpawnCount - 1)));
                speed->speed         = std::min(speedCalculate, 135.0f);
            }
            if (attack)  // +20% attack mỗi lần spawn
            {
                attack->baseDamage = attack->baseDamage * (1.0 + (0.2 * (bossSpawnCount - 1)));
            }
            isBossActive = true;

            bossList.push_back(boss);
        }
    }

    // Gọi cảnh báo boss
    dynamic_cast<GameSceneUILayer*>(SystemManager::getInstance()->getSceneLayer())->bossAlert();
}

void SpawnSystem::isBossAlive(bool isBossActive)
{
    this->isBossActive = isBossActive;
}

Entity SpawnSystem::spawnEntity(const std::string& type, const std::string& name, const ax::Vec2& position)
{
    float elapsedTime = SystemManager::getInstance()->getSystem<TimeSystem>()->getElapsedTime();
    Entity entity     = factory->createEntity(type, name);
    ax::Vec2 spawnPos(getRandomSpawnPosition(entity, position));
    if (entity)
    {
        if (auto transform = transformMgr.getComponent(entity))
        {
            transform->x = spawnPos.x;
            transform->y = spawnPos.y;
        }
        if (auto spriteComp = spriteMgr.getComponent(entity))
        {
            spriteComp->initializeSprite();
        }
        if (auto animationComp = animationMgr.getComponent(entity))
        {
            animationComp->initializeAnimations();
            animationComp->currentState = "idle";
        }
        if (type != "boss")
        {
            auto health   = healthMgr.getComponent(entity);
            auto speed    = speedMgr.getComponent(entity);
            auto attack   = attackMgr.getComponent(entity);
            auto identity = identityMgr.getComponent(entity);

            int statsMultiplier = elapsedTime / 30;  // 30s tăng stats enemy 1 lần

            health->maxHealth     = health->maxHealth * (1 + (0.5 * statsMultiplier));  // 50% mỗi lần tăng
            health->currentHealth = health->maxHealth;

            attack->baseDamage = attack->baseDamage * (1 + (0.1 * statsMultiplier));  // 10% mỗi lần tăng

            if (identity->name != "Octopus")
            {
                float speedMultiplier = std::min(1 + (0.05 * statsMultiplier), 2.5);  // 5% mỗi lần tăng
                float speedCalculate  = speed->speed * speedMultiplier;
                speed->speed          = std::min(speedCalculate, 500.0f);
            }
            // Thêm entity vào batch để cập nhật di chuyển (cần xóa khi chết)
            SystemManager::getInstance()->getSystem<MovementSystem>()->assignEntityToBatch(entity);
            // Xóa tại HealthSystem
            // Xóa entity khỏi batch cập nhật di chuyển khi chết
            // SystemManager::getInstance()->getSystem<MovementSystem>()->updateBatchOnDeath(deadEnemyEntity);
        }
    }

    return entity;
}

ax::Vec2 SpawnSystem::getRandomSpawnPosition(Entity entity, const ax::Vec2& playerPosition)
{
    const float VIEW_WIDTH   = 360.0f;  // Chiều rộng view
    const float VIEW_HEIGHT  = 640.0f;  // Chiều cao view
    const float SPAWN_MARGIN = 200.0f;  // Khoảng cách spawn bên ngoài viền

    // Kích thước vùng spawn (view + margin hai bên)
    const float spawnWidth  = VIEW_WIDTH + 2 * SPAWN_MARGIN;
    const float spawnHeight = VIEW_HEIGHT + 2 * SPAWN_MARGIN;

    // Tọa độ vùng view (không spawn trong này)
    float viewMinX = playerPosition.x - VIEW_WIDTH / 2;   // player.x - 180
    float viewMaxX = playerPosition.x + VIEW_WIDTH / 2;   // player.x + 180
    float viewMinY = playerPosition.y - VIEW_HEIGHT / 2;  // player.y - 320
    float viewMaxY = playerPosition.y + VIEW_HEIGHT / 2;  // player.y + 320

    auto mapSystem  = SystemManager::getInstance()->getSystem<MapSystem>();
    float mapWidth  = mapSystem->getChunkSize().width * mapSystem->getGridSize().width;
    float mapHeight = mapSystem->getChunkSize().height * mapSystem->getGridSize().height;

    ax::Vec2 spawnPosition;
    std::vector<int> availableZones = {0, 1, 2, 3};  // Danh sách các vùng spawn (chia 4 phần)

    while (!availableZones.empty())
    {
        // Chọn ngẫu nhiên 1 vùng từ danh sách còn lại
        int index     = rand() % availableZones.size();
        int spawnZone = availableZones[index];
        availableZones.erase(availableZones.begin() + index);  // Xóa vùng đã thử khỏi danh sách

        switch (spawnZone)
        {
        case 0:  // Bên trái
            spawnPosition.x = viewMinX - SPAWN_MARGIN + static_cast<float>(rand()) / RAND_MAX * SPAWN_MARGIN;
            spawnPosition.y = viewMinY - SPAWN_MARGIN + static_cast<float>(rand()) / RAND_MAX * spawnHeight;
            break;

        case 1:  // Bên phải
            spawnPosition.x = viewMaxX + static_cast<float>(rand()) / RAND_MAX * SPAWN_MARGIN;
            spawnPosition.y = viewMinY - SPAWN_MARGIN + static_cast<float>(rand()) / RAND_MAX * spawnHeight;
            break;

        case 2:  // Bên trên
            spawnPosition.x = viewMinX + static_cast<float>(rand()) / RAND_MAX * VIEW_WIDTH;
            spawnPosition.y = viewMaxY + static_cast<float>(rand()) / RAND_MAX * SPAWN_MARGIN;
            break;

        case 3:  // Bên dưới
            spawnPosition.x = viewMinX + static_cast<float>(rand()) / RAND_MAX * VIEW_WIDTH;
            spawnPosition.y = viewMinY - SPAWN_MARGIN + static_cast<float>(rand()) / RAND_MAX * SPAWN_MARGIN;
            break;
        }

        // Giới hạn trong map
        spawnPosition.x = std::max(0.0f, std::min(spawnPosition.x, mapWidth));
        spawnPosition.y = std::max(0.0f, std::min(spawnPosition.y, mapHeight));

        // Nếu không có va chạm với tường, trả về vị trí spawn hợp lệ
        if (!isSpawnOnCollisionTile(entity, spawnPosition))
        {
            return spawnPosition;
        }
    }

    // Nếu không tìm thấy vị trí hợp lệ, chọn vị trí gần viền map
    // AXLOG("Warning: Could not find valid spawn position, using fallback.");
    spawnPosition.x = std::max(0.0f, std::min(viewMinX - SPAWN_MARGIN, mapWidth));
    spawnPosition.y = std::max(0.0f, std::min(viewMinY - SPAWN_MARGIN, mapHeight));
    return spawnPosition;
}

bool SpawnSystem::isSpawnOnCollisionTile(Entity entity, const ax::Vec2& spawnPosition)
{
    auto collisionSystem = SystemManager::getInstance()->getSystem<CollisionSystem>();
    return collisionSystem->isCollidingWithTileMap(entity, spawnPosition);
}

// Lấy vị trí world của player
ax::Vec2 SpawnSystem::getPlayerPosition() const
{
    auto transform = transformMgr.getComponent(playerEntity);
    if (transform)
    {
        return ax::Vec2(transform->x, transform->y);
    }
    return ax::Vec2::ZERO;
}
