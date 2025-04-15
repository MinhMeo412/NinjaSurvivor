#include "SpawnSystem.h"

#include "SystemManager.h"
#include "GameData.h"
#include "EntityFactory.h"
#include "MapSystem.h"
#include "TimeSystem.h"
#include "CollisionSystem.h"
#include "MovementSystem.h"


void SpawnSystem::init()
{
    // Khởi tạo entity factory
    factory = std::make_unique<EntityFactory>(entityManager, identityMgr, transformMgr, spriteMgr, animationMgr,
                                              velocityMgr, hitboxMgr, healthMgr, attackMgr, cooldownMgr, speedMgr, weaponInventoryMgr, durationMgr);

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


    //Test
    int gridSize = 2;
    int spacing  = 16;
    for (int i = 0; i < 4; i++)
    {
        Entity entity = factory->createEntity("enemy", "Slime");
        ax::Vec2 spawnPos(800,1000);
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
    if (spawnTimer >= spawnInterval)
    {
        spawnEnemies(elapsedTime);
        spawnBoss(elapsedTime);
        spawnTimer = 0.0f;  // Reset timer
    }
}

void SpawnSystem::spawnEnemies(float elapsedTime)
{

    int numEnemies;  // Số enemy spawn
    int minute = elapsedTime / 60;
    numEnemies = 2 + minute;

    // if (elapsedTime < 30)  // 0-1 phút
    //{
    //     numEnemies = 2 + rand() % 2;  // 2-3 quái
    // }
    // else if (elapsedTime < 60)  // 0-1 phút
    //{
    //     numEnemies = 3 + rand() % 3;  // 3-5 quái
    // }
    // else if (elapsedTime < 180)  // 1-3 phút
    //{
    //     numEnemies = 5 + rand() % 4;  // 5-8 quái
    // }
    // else if (elapsedTime < 420)  // 3-7 phút
    //{
    //     numEnemies = 10 + rand() % 6;  // 10-15 quái
    // }
    // else if (elapsedTime < 600)  // 7-10 phút
    //{
    //     numEnemies = 20 + rand() % 11;  // 20-30 quái
    // }
    // else  // >10 phút
    //{
    //     numEnemies = 35 + rand() % 16;  // 35-50 quái
    // }

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
    float spawnRateMultiplier = isBossActive ? 0.3f : 1.0f;
    numEnemies                = static_cast<int>(numEnemies * spawnRateMultiplier);

    // Tỷ lệ spawn từng loại quái
    float slimeRatio = 0.0, snakeRatio = 0.0, bearRatio = 0.0, octopusRatio = 0.0;
    if (elapsedTime < 60)  // 0-1 phút
    {
        slimeRatio = 1.0f;
    }
    else if (elapsedTime < 180)  // 1-3 phút
    {
        slimeRatio = 0.9f;
        bearRatio  = 0.1f;
    }
    else if (elapsedTime < 420)  // 3-7 phút
    {
        slimeRatio = 0.5f;
        snakeRatio = 0.2f;
        bearRatio  = 0.3f;
    }
    else if (elapsedTime < 600)  // 7-10 phút
    {
        slimeRatio   = 0.3f;
        snakeRatio   = 0.3f;
        bearRatio    = 0.3f;
        octopusRatio = 0.1f;
    }
    else  // >10 phút
    {
        snakeRatio   = 0.2f;
        bearRatio    = 0.7f;
        octopusRatio = 0.1f;
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
                health->maxHealth *= 2.0f;
                health->currentHealth = health->maxHealth;
            }
            if (auto speed = speedMgr.getComponent(enemy))
            {
                speed->speed = std::min(speed->speed * 2.0f, 300.0f);
            }
        }

        livingEnemyCount++;  // Tăng số lượng enemy sống
    }
}

void SpawnSystem::spawnBoss(float elapsedTime)
{
    if (static_cast<int>(elapsedTime) % 300 == 0 && elapsedTime > 0)
    {  // Mỗi 5 phút
        TransformComponent* playerTransform = transformMgr.getComponent(playerEntity);
        if (!playerTransform)
            return;
        ax::Vec2 playerPos = ax::Vec2(playerTransform->x, playerTransform->y);

        Entity boss = spawnEntity("enemy", "Slime", playerPos);  // dùng tạm Slime làm boss
        if (boss)
        {
            if (elapsedTime < 610)  // boss tại phút 10
            {
                auto health = healthMgr.getComponent(boss);
                if (health)
                {
                    health->maxHealth     = health->maxHealth * 1.2;
                    health->currentHealth = health->maxHealth;
                }
                if (auto speed = speedMgr.getComponent(boss))
                {
                    speed->speed = std::min(speed->speed * 1.2f, 200.0f);
                }
                if (auto attack = attackMgr.getComponent(boss))
                {
                    attack->baseDamage *= 1.5f;
                }
            }
            else if (elapsedTime < 910)  // boss tại phút 15
            {
                auto health = healthMgr.getComponent(boss);
                if (health)
                {
                    health->maxHealth     = health->maxHealth * 1.5;
                    health->currentHealth = health->maxHealth;
                }
                if (auto speed = speedMgr.getComponent(boss))
                {
                    speed->speed = std::min(speed->speed * 1.5f, 200.0f);
                }
                if (auto attack = attackMgr.getComponent(boss))
                {
                    attack->baseDamage *= 2.0f;
                }
            }
            else  // boss tại phút 15 trở đi
            {
                auto health = healthMgr.getComponent(boss);
                if (health)
                {
                    health->maxHealth =
                        health->maxHealth * (2 + (elapsedTime - 1200) / 300);  // Boss trâu hơn theo thời gian
                    health->currentHealth = health->maxHealth;
                }
                if (auto speed = speedMgr.getComponent(boss))
                {
                    speed->speed = std::min(speed->speed * 1.5f, 200.0f);
                }
                if (auto attack = attackMgr.getComponent(boss))
                {
                    attack->baseDamage *= 2.0f;
                }
            }

            isBossActive = true;
        }
    }
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

        if (elapsedTime > 300 && elapsedTime < 600)  // 5-10 phút
        {
            if (auto health = healthMgr.getComponent(entity))
            {
                health->maxHealth *= 2.0f;
                health->currentHealth = health->maxHealth;
            }
        }
        else if (elapsedTime > 600 && elapsedTime < 900)  // 10-15 phút
        {
            if (auto health = healthMgr.getComponent(entity))
            {
                health->maxHealth *= 3.0f;
                health->currentHealth = health->maxHealth;
            }
            if (auto attack = attackMgr.getComponent(entity))
            {
                attack->baseDamage *= 1.5f;
            }
        }
        else if (elapsedTime > 900)  // >15 phút
        {
            if (auto health = healthMgr.getComponent(entity))
            {
                health->maxHealth *= 4.0f;
                health->currentHealth = health->maxHealth;
            }
            if (auto speed = speedMgr.getComponent(entity))
            {
                speed->speed = std::min(speed->speed * 1.5f, 300.0f);
            }
            if (auto attack = attackMgr.getComponent(entity))
            {
                attack->baseDamage *= 2.0f;
            }
        }
    }

    // Thêm entity vào batch để cập nhật di chuyển (cần xóa khi chết)
    SystemManager::getInstance()->getSystem<MovementSystem>()->assignEntityToBatch(entity);
    // Xóa tại HealthSystem
    // Xóa entity khỏi batch cập nhật di chuyển khi chết
    // SystemManager::getInstance()->getSystem<MovementSystem>()->updateBatchOnDeath(deadEnemyEntity);

    return entity;
}

ax::Vec2 SpawnSystem::getRandomSpawnPosition(Entity entity, const ax::Vec2& playerPosition)
{
    const float VIEW_WIDTH   = 360.0f;   // Chiều rộng view
    const float VIEW_HEIGHT  = 640.0f;  // Chiều cao view
    const float SPAWN_MARGIN = 300.0f;   // Khoảng cách spawn bên ngoài viền

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
    //AXLOG("Warning: Could not find valid spawn position, using fallback.");
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
