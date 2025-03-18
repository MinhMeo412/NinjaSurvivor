#include "MovementSystem.h"

#include "JoystickSystem.h"
#include "SpawnSystem.h"
#include "MapSystem.h"
#include "SystemManager.h"

MovementSystem::MovementSystem(EntityManager& em,
                               ComponentManager<TransformComponent>& tm,
                               ComponentManager<VelocityComponent>& vm,
                               ComponentManager<AnimationComponent>& am,
                               ComponentManager<HitboxComponent>& hm)
    : entityManager(em), transformMgr(tm), velocityMgr(vm), animationMgr(am), hitboxMgr(hm) {}

void MovementSystem::init() {}

void MovementSystem::update(float dt)
{
    auto systemManager  = SystemManager::getInstance();
    auto joystickSystem = systemManager->getSystem<JoystickSystem>();
    auto spawnSystem    = systemManager->getSystem<SpawnSystem>();
    auto mapSystem      = systemManager->getSystem<MapSystem>();

    if (!spawnSystem)
    {
        AXLOG("spawnSystem null");
        return;
    }

    Entity playerEntity = spawnSystem->getPlayerEntity();
    auto entities       = entityManager.getActiveEntities();

    for (Entity entity : entities)
    {
        if (auto transform = transformMgr.getComponent(entity))
        {
            if (auto velocity = velocityMgr.getComponent(entity))
            {
                if (entity == playerEntity && joystickSystem)
                {
                    ax::Vec2 joystickDirection = joystickSystem->getDirection();
                    velocity->vx               = joystickDirection.x * 150.0f;  // Sử dụng speed
                    velocity->vy               = joystickDirection.y * 150.0f;

                    if (auto animation = animationMgr.getComponent(entity))
                    {
                        // Chuyển đổi vector direction thành hệ góc (-180;180)
                        float angle = AX_RADIANS_TO_DEGREES(atan2(joystickDirection.y, joystickDirection.x));
                        if (angle < 0) angle += 360.0f; //Nếu là góc âm chuyển thành hệ góc (0;360)

                        // Kiểm tra góc cho "moveDown" (255° đến 285°)
                        if (angle >= 255.0f && angle <= 285.0f)
                        {
                            animation->currentState = "moveDown";
                        }
                        // Nếu vx âm thì "moveLeft"
                        else if (velocity->vx < 0)
                        {
                            animation->currentState = "moveLeft";
                        }
                        // Nếu vx dương thì "moveRight"
                        else if (velocity->vx > 0)
                        {
                            animation->currentState = "moveRight";
                        }
                        // Mặc định (= 0)
                        else
                        {
                            animation->currentState = "idle";
                        }
                    }
                }
                else
                {
                    auto playerPos = transformMgr.getComponent(playerEntity);
                    auto enemyPos  = transform;

                    ax::Vec2 direction = ax::Vec2((playerPos->x - enemyPos->x), (playerPos->y - enemyPos->y));

                    if (!direction.isZero())
                    {
                        direction.normalize();
                    }

                    // Calculate the new position by moving in that direction
                    ax::Vec2 newPos = direction * 50.0f;

                    velocity->vx = newPos.x;
                    velocity->vy = newPos.y;

                    if (auto animation = animationMgr.getComponent(entity))
                    {
                        if (velocity->vx > 0)
                            animation->currentState = "moveRight";  //"moveRight";
                        else if (velocity->vx < 0)
                            animation->currentState = "moveLeft";  //"moveLeft";
                        else if (velocity->vy > 0)
                            animation->currentState = "moveDown";  // Move up changes
                        else if (velocity->vy < 0)
                            animation->currentState = "moveDown";  //"moveDown";
                        else
                            animation->currentState = "idle";
                    }

                    // AXLOG("Not found any else entity");
           
                }

                // Tính toán sub-stepping và va chạm
                ax::Vec2 currentPos = ax::Vec2(transform->x, transform->y);
                ax::Vec2 moveStep   = ax::Vec2(velocity->vx, velocity->vy) * dt;
                float stepLength    = moveStep.length();
                ax::Size tileSize   = mapSystem->getTileSize();
                int steps           = std::ceil(stepLength / (tileSize.width * 0.5f));
                ax::Vec2 step       = moveStep / (steps > 0 ? steps : 1);

                ax::Vec2 adjustedPos = currentPos;
                for (int i = 0; i < steps; ++i)
                {
                    adjustedPos = resolveCollision(entity, adjustedPos, step);
                }

                //Di chuyển
                transform->x = adjustedPos.x;
                transform->y = adjustedPos.y;

                // Gọi callback cho MapSystem nếu là player
                if (entity == playerEntity)
                {
                    mapSystem->onPlayerPositionChanged(adjustedPos);
                }
            }
        }
    }
}

ax::Vec2 MovementSystem::resolveCollision(Entity entity, const ax::Vec2& currentPos, const ax::Vec2& moveStep)
{
    ax::Vec2 newPos = currentPos + moveStep;
    if (!isCollidingWithTileMap(entity, newPos))
        return newPos;

    ax::Vec2 testPosX = currentPos + ax::Vec2(moveStep.x, 0);
    ax::Vec2 testPosY = currentPos + ax::Vec2(0, moveStep.y);
    bool canMoveX     = !isCollidingWithTileMap(entity, testPosX);
    bool canMoveY     = !isCollidingWithTileMap(entity, testPosY);

    if (canMoveX)
        return testPosX;
    if (canMoveY)
        return testPosY;
    return currentPos;
}

bool MovementSystem::isCollidingWithTileMap(Entity entity, const ax::Vec2& position)
{
    auto mapSystem = SystemManager::getInstance()->getSystem<MapSystem>();
    if (!mapSystem)
        return false;

    auto hitbox = hitboxMgr.getComponent(entity);
    if (!hitbox)
        return false;

    ax::Vec2 points[4] = {position + ax::Vec2(-hitbox->size.width / 2, -hitbox->size.height / 2),
                          position + ax::Vec2(hitbox->size.width / 2, -hitbox->size.height / 2),
                          position + ax::Vec2(-hitbox->size.width / 2, hitbox->size.height / 2),
                          position + ax::Vec2(hitbox->size.width / 2, hitbox->size.height / 2)};

    for (const ax::Vec2& point : points)
    {
        // Lấy chunk chứa điểm
        ax::TMXTiledMap* tiledMap = getChunkTileMap(point);
        if (!tiledMap)
            continue;

        auto collisionLayer = tiledMap->getLayer("CollisionLayer");
        if (!collisionLayer || !collisionLayer->getTiles()) // Kiểm tra layer tồn tại và có tile
            continue;


        ax::Vec2 tileCoord = convertToTileCoord(point, tiledMap);
        ax::Size mapSize   = tiledMap->getMapSize();  
        if (tileCoord.x >= 0 && tileCoord.x < mapSize.width && tileCoord.y >= 0 && tileCoord.y < mapSize.height &&
            collisionLayer->getTileGIDAt(tileCoord) != 0) //một lần kiểm tra cả 4 góc hitbox với getTileGIDAt (tốn CPU) / cần tìm cách khác
        {  // Kiểm tra tile có GID (khác 0 là có tile)
            return true;
        }
    }
    return false;
}

ax::Vec2 MovementSystem::convertToTileCoord(const ax::Vec2& position, ax::TMXTiledMap* tiledMap)
{
    ax::Vec2 mapPos   = tiledMap->getPosition();
    ax::Size tileSize = tiledMap->getTileSize();
    ax::Size mapSize  = tiledMap->getMapSize();

    float x   = (position.x - mapPos.x) / tileSize.width;
    float y   = (position.y - mapPos.y) / tileSize.height;
    int tileX = std::floor(x);
    int tileY = std::floor(mapSize.height - y);  // Đảo ngược Y để khớp với TMX

    return ax::Vec2(tileX, tileY);
}


ax::TMXTiledMap* MovementSystem::getChunkTileMap(const ax::Vec2& position)
{
    auto mapSystem = SystemManager::getInstance()->getSystem<MapSystem>();
    if (!mapSystem)
        return nullptr;

    // Tính chunk từ vị trí thế giới
    int chunkX = std::floor(position.x / mapSystem->getChunkSize().x);
    int chunkY = std::floor((mapSystem->getGridSize().y * mapSystem->getChunkSize().y - position.y - 1) /
                            mapSystem->getChunkSize().y);  // Lật Y
    if (chunkX >= 0 && chunkX < mapSystem->getGridSize().x && chunkY >= 0 && chunkY < mapSystem->getGridSize().y)
    {
        const auto& grid = mapSystem->getGrid();
        return grid[chunkY][chunkX].tiledMap;
    }
    return nullptr;
}
