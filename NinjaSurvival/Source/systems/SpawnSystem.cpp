#include "SpawnSystem.h"

#include "SystemManager.h"
#include "GameData.h"
#include "EntityFactory.h"
#include "TimeSystem.h"


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
    if (spawnTimer >= spawnInterval)
    {
        spawnEnemies(elapsedTime);
        spawnBoss(elapsedTime);
        spawnTimer = 0.0f;  // Reset timer
    }
}

void SpawnSystem::spawnEnemies(float elapsedTime)
{
    int maxEnemies = 300;                         // Giới hạn tối đa enemy sống
    int numEnemies = 30 + (elapsedTime / 60) * 2;  // Tăng số quái theo phút

    // Giới hạn spawn khi vượt quá 300 enemy
    if (livingEnemyCount >= maxEnemies)
    {
        numEnemies = std::min(numEnemies, 2);  // Chỉ spawn tối đa 2 con
    }

    // Điều chỉnh tỷ lệ spawn khi boss xuất hiện (hoặc cho = 0 để ko spawn)
    float spawnRateMultiplier = isBossActive ? 0.5f : 1.0f;
    numEnemies                = static_cast<int>(numEnemies * spawnRateMultiplier);

    // Tỷ lệ spawn từng loại quái
    float slimeRatio, snakeRatio, bearRatio, octopusRatio;
    if (elapsedTime < 300)
    {  // 0 - 5 phút
        //slimeRatio   = 0.7;
        slimeRatio   = 1.0; //test
        //snakeRatio = 0.2;
        //bearRatio    = 0.1;
        snakeRatio   = 0.0;
        bearRatio    = 0.0;
        octopusRatio = 0.0;
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
    TransformComponent* playerPos = transformMgr.getComponent(playerEntity);
    if (!playerPos)
        return;

    for (int i = 0; i < numEnemies; i++)
    {
        float randVal     = static_cast<float>(rand()) / RAND_MAX;  // Số ngẫu nhiên từ 0.0 - 1.0
        ax::Vec2 spawnPos = getRandomSpawnPosition(playerPos, 400.0f, 600.0f);

        Entity enemy;
        if (randVal < slimeRatio)
        {
            enemy = spawnEntity("enemy", "Slime", spawnPos);
        }
        else if (randVal < slimeRatio + snakeRatio)
        {
            enemy = spawnEntity("enemy", "Snake", spawnPos);
        }
        else if (randVal < slimeRatio + snakeRatio + bearRatio)
        {
            enemy = spawnEntity("enemy", "Bear", spawnPos);
        }
        else
        {
            enemy = spawnEntity("enemy", "Octopus", spawnPos);
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
        TransformComponent* playerPos = transformMgr.getComponent(playerEntity);
        if (!playerPos)
            return;

        ax::Vec2 spawnPos = getRandomSpawnPosition(playerPos, 300.0f, 500.0f);
        Entity boss = spawnEntity("enemy", "Slime", spawnPos);  // dùng tạm Slime làm boss
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
    if (entity)
    {
        if (auto transform = transformMgr.getComponent(entity))
        {
            transform->x = position.x;
            transform->y = position.y;
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

ax::Vec2 SpawnSystem::getRandomSpawnPosition(TransformComponent* playerPosition, float innerRadius, float outerRadius)
{
    // Generate a random angle between 0 and 2π.
    float theta = static_cast<float>(rand()) / RAND_MAX * 2.0f * M_PI;

    // Generate a uniform random number between 0 and 1.
    float u = static_cast<float>(rand()) / RAND_MAX;

    // Compute the radius so that spawn points are uniformly distributed in area.
    float r = sqrt(u * (outerRadius * outerRadius - innerRadius * innerRadius) + innerRadius * innerRadius);

    // Calculate spawn position
    ax::Vec2 spawnPosition;
    spawnPosition.x = playerPosition->x + r * cos(theta);
    spawnPosition.y = playerPosition->y + r * sin(theta);

    return spawnPosition;
}



//    for (int i = 0; i < 10; i++)
//{
//    Entity enemyEntity = factory.createEntity("enemy", "Slime");
//    if (enemyEntity != 0)
//    {
//        if (auto transform = transformMgr.getComponent(enemyEntity))
//        {
//            auto playerPos  = transformMgr.getComponent(playerEntity);
//            auto spawnPoint = getRandomSpawnPosition(playerPos, 100.0f, 200.0f);
//            transform->x    = spawnPoint.x;
//            transform->y    = spawnPoint.y;
//            AXLOG("SpawnSystem: Created enemy Slime with ID %u at (%f, %f)", enemyEntity, transform->x, transform->y);
//        }
//        if (auto animationComp = animationMgr.getComponent(enemyEntity))
//        {
//            animationComp->initializeAnimations();
//            animationComp->currentState = "idle";
//            AXLOG("Spawned enemy animation for entity %u", enemyEntity);
//        }
//        if (auto spriteComp = spriteMgr.getComponent(enemyEntity))
//        {
//            spriteComp->initializeSprite();
//            AXLOG("Spawned enemy sprite for entity %u", enemyEntity);
//        }
//    }
//}
