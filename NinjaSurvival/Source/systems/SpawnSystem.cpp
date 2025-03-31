#include "SpawnSystem.h"

#include "SystemManager.h"
#include "GameData.h"
#include "EntityFactory.h"
#include "MapSystem.h"
#include "TimeSystem.h"
#include "CollisionSystem.h"


void SpawnSystem::init()
{
    std::string characterName = GameData::getInstance()->getSelectedCharacter();
    if (characterName.empty())
    {
        AXLOG("Error: No character selected in GameData");
        return;
    }

    EntityFactory factory(entityManager, identityMgr, transformMgr, spriteMgr, animationMgr, velocityMgr, hitboxMgr,
                          healthMgr, attackMgr, cooldownMgr, speedMgr);
    playerEntity = factory.createEntity("player", characterName);

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
    //if (spawnTimer >= spawnInterval)
    //{
    //    spawnEnemies(elapsedTime);
    //    spawnBoss(elapsedTime);
    //    spawnTimer = 0.0f;  // Reset timer
    //}
}

void SpawnSystem::spawnEnemies(float elapsedTime)
{
    int maxEnemies = 300;                         // Giới hạn tối đa enemy sống
    int numEnemies = 2 + (elapsedTime / 60) * 2;  // Tăng số quái theo phút

    // Giới hạn spawn khi vượt quá 300 enemy
    if (livingEnemyCount >= maxEnemies)
    {
        numEnemies = std::min(numEnemies, 2);  // Chỉ spawn tối đa 2 con
    }

    // Điều chỉnh tỷ lệ spawn khi boss xuất hiện (hoặc cho = 0 để ko spawn)
    float spawnRateMultiplier = isBossActive ? 0.5f : 1.0f;
    numEnemies                = static_cast<int>(numEnemies * spawnRateMultiplier);

    // Tỷ lệ spawn từng loại quái
    float slimeRatio = 0.0, snakeRatio = 0.0, bearRatio = 0.0, octopusRatio = 0.0;
    if (elapsedTime < 300)
    {  // 0 - 5 phút
        //slimeRatio   = 0.7;
        //slimeRatio   = 1.0; //test
        //snakeRatio = 0.2;
        //bearRatio    = 0.1;
        octopusRatio = 1.0;
    }
    else if (elapsedTime < 600)
    {  // 5 - 10 phút
        slimeRatio = 0.5;
        snakeRatio = 0.2;
        bearRatio  = 0.2;
        octopusRatio = 0.1;
    }
    else if (elapsedTime < 900)
    {  // 10 - 15 phút
        slimeRatio = 0.25;
        snakeRatio = 0.15;
        bearRatio  = 0.35;
        octopusRatio = 0.25;
    }
    else 
    { // >15 phút
        slimeRatio   = 0.1;
        snakeRatio   = 0.1;
        bearRatio    = 0.7;
        octopusRatio = 0.1;
    }

    //Lấy vị trí player
    TransformComponent* playerTransform = transformMgr.getComponent(playerEntity);
    if (!playerTransform)
        return;
    ax::Vec2 playerPos = ax::Vec2(playerTransform->x, playerTransform->y);

    for (int i = 0; i < numEnemies; i++)
    {
        float randVal     = static_cast<float>(rand()) / RAND_MAX;  // Số ngẫu nhiên từ 0.0 - 1.0
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

        if (enemy && isBossActive)
        {
            HealthComponent* health = healthMgr.getComponent(enemy);
            if (health)
                health->maxHealth *= 1.5f;  // Tăng HP khi boss xuất hiện
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
            HealthComponent* health = healthMgr.getComponent(boss);
            if (health)
            {
                health->maxHealth     = 1000 + (elapsedTime / 60) * 200;  // Boss trâu hơn theo thời gian
                health->currentHealth = health->maxHealth;
            }
            VelocityComponent* velocity = velocityMgr.getComponent(boss);
            if (velocity)
            {
                float bossSpeed = 0.5f - (elapsedTime / 1800.0f);  // Chậm dần
                velocity->vx    = bossSpeed * 200;
            }
            isBossActive = true;
        }
    }
}

void SpawnSystem::adjustEnemySpawnDuringBoss(bool isBossActive)
{
    this->isBossActive = isBossActive;
}

Entity SpawnSystem::spawnEntity(const std::string& type, const std::string& name, const ax::Vec2& position)
{
    EntityFactory factory(entityManager, identityMgr, transformMgr, spriteMgr, animationMgr, velocityMgr, hitboxMgr,
                          healthMgr, attackMgr, cooldownMgr, speedMgr);
    Entity entity = factory.createEntity(type, name);
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
    }
    return entity;
}

ax::Vec2 SpawnSystem::getRandomSpawnPosition(Entity entity, const ax::Vec2& playerPosition)
{
    const float VIEW_WIDTH   = 720.0f;   // Chiều rộng view
    const float VIEW_HEIGHT  = 1280.0f;  // Chiều cao view
    const float SPAWN_MARGIN = 300.0f;   // Khoảng cách spawn bên ngoài viền

    // Kích thước vùng spawn (view + margin hai bên)
    const float spawnWidth  = VIEW_WIDTH + 2 * SPAWN_MARGIN;   // 1320
    const float spawnHeight = VIEW_HEIGHT + 2 * SPAWN_MARGIN;  // 1880

    // Tọa độ vùng view (không spawn trong này)
    float viewMinX = playerPosition.x - VIEW_WIDTH / 2;   // player.x - 360
    float viewMaxX = playerPosition.x + VIEW_WIDTH / 2;   // player.x + 360
    float viewMinY = playerPosition.y - VIEW_HEIGHT / 2;  // player.y - 640
    float viewMaxY = playerPosition.y + VIEW_HEIGHT / 2;  // player.y + 640

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
    AXLOG("Warning: Could not find valid spawn position, using fallback.");
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
    return ax::Vec2::ZERO;  // Trả về (0, 0) nếu không có player
}
