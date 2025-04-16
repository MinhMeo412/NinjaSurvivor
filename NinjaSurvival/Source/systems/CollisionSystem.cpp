#include "CollisionSystem.h"
#include "SpawnSystem.h"
#include "MapSystem.h"
#include "HealthSystem.h"
#include "WeaponSystem.h"
#include "CleanupSystem.h"
#include "SystemManager.h"

#include "Utils.h"

CollisionSystem::CollisionSystem(EntityManager& em,
                                 ComponentManager<IdentityComponent>& im,
                                 ComponentManager<TransformComponent>& tm,
                                 ComponentManager<HitboxComponent>& hm)
    : entityManager(em), identityMgr(im), transformMgr(tm), hitboxMgr(hm)
{}

void CollisionSystem::init()
{
    // Lấy hệ thống MapSystem từ SystemManager
    auto mapSystem = SystemManager::getInstance()->getSystem<MapSystem>();
    if (mapSystem)
    {
        // Khởi tạo SpatialGrid với kích thước toàn bộ map
        spatialGrid.init(mapSystem->getGridSize() * mapSystem->getChunkSize());
    }

    //// Đặt callback logic khi va chạm xảy ra
    onCollision = [](Entity e1, Entity e2)
        {
            AXLOG("Collision between %u and %u", e1, e2);
            //Xử lý logic va chạm
            auto healthSystem = SystemManager::getInstance()->getSystem<HealthSystem>();
            healthSystem->handleCollision(e1, e2);
        };

    onWeaponCollision = [](Entity e1, Entity e2) {
        AXLOG("Collision between %u and %u", e1, e2);
        // Xử lý logic va chạm
        auto healthSystem = SystemManager::getInstance()->getSystem<HealthSystem>();
        
        healthSystem->damagedEnemy[e1].push_back(e2);
        healthSystem->handleWeaponCollision(healthSystem->damagedEnemy);
        for (const auto& [attacker, targets] : healthSystem->damagedEnemy)
        {
            AXLOG("Attacker Entity: %u", attacker);

            for (const auto& target : targets)
            {
                AXLOG("  -> Damaged Target: %u", target);
            }
        }
    };
}

// cập nhật hệ thống va chạm, gọi mỗi frame
void CollisionSystem::update(float dt)
{
    auto start       = std::chrono::high_resolution_clock::now();

    auto spawnSystem = SystemManager::getInstance()->getSystem<SpawnSystem>();
    if (!spawnSystem)
        return;

    Entity player = spawnSystem->getPlayerEntity();// Lấy entity của player
    spatialGrid.clear();// Xóa toàn bộ entity trong lưới
    // Thêm tất cả entity đang hoạt động vào lưới
    for (Entity e : entityManager.getActiveEntities())
    {
        if (auto t = transformMgr.getComponent(e))
            spatialGrid.insert(e, ax::Vec2(t->x, t->y));
    }

    // Kiểm tra va chạm giữa player và các entity khác
    auto entitiesNearby = spatialGrid.getNearbyEntities(spawnSystem->getPlayerPosition(),1); //Va chạm với boss có hitbox lớn bị miss
    for (Entity entity : entitiesNearby)
    {
        auto entityType = identityMgr.getComponent(entity)->type;
        //Kiểm tra có va chạm không
        if (entity != player && Utils::not_in(entityType, "weapon_melee", "weapon_projectile", "item") &&
            checkCollision(player, entity))
        {
            if (onCollision) // Kiểm tra đã được gán callback hay chưa
                onCollision(player, entity); // Gọi callback
        }
    }

    //Kiểm tra va chạm giữa weapon trong pool với enemy/boss
    weaponCollisionCheck();

    auto end      = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    //AXLOG("Thời gian thực thi CollisionSystem: %ld ms", duration);
}

// Hàm xử lý vị trí mới của entity khi di chuyển
ax::Vec2 CollisionSystem::resolvePosition(Entity entity, const ax::Vec2& newPos)
{
    auto identity = identityMgr.getComponent(entity);
    if (!identity)
        return newPos;  // Trả về vị trí mới nếu không có identity

    std::string type = identity->type;
    // Lấy vị trí hiện tại của entity
    ax::Vec2 currentPos = transformMgr.getComponent(entity)
                              ? ax::Vec2(transformMgr.getComponent(entity)->x, transformMgr.getComponent(entity)->y)
                              : newPos;
    ax::Vec2 moveStep   = newPos - currentPos;  // Tính bước di chuyển

    // Enemy/Boss: Không overlay với enemy/boss, chặn bởi collision tile
    if (Utils::in(type, "enemy", "boss"))
    {
        bool collidesWithTile =
            isCollidingWithTileMap(entity, newPos);  // Kiểm tra xem có va chạm với tilemap tại newPos không
        ax::Vec2 adjustedPos = newPos;
        bool collidesWithEntity =
            false;  // Định nghĩa biến: xác định có va chạm với entity khác hay không (mặc định false)
        ax::Vec2 pushDirection(0, 0);  // hướng đẩy
        float pushStrength = 2.0f;     // lực đẩy

        // Kiểm tra va chạm với entity khác
        auto nearby = spatialGrid.getNearbyEntities(newPos,1);
        for (Entity other : nearby)
        {
            if (other != entity && identityMgr.getComponent(other))
            {
                std::string otherType = identityMgr.getComponent(other)->type;
                // Kiểm tra type nếu là enemy hoặc boss và có va chạm
                if (Utils::in(otherType, "enemy", "boss") && checkCollision(entity, other))
                {
                    collidesWithEntity = true;
                    ax::Vec2 otherPos(transformMgr.getComponent(other)->x, transformMgr.getComponent(other)->y);
                    pushDirection += (currentPos - otherPos);
                    // Hướng đẩy bằng hiệu currentPos - otherPos, tức là hướng di chuyển ra xa khỏi other
                }
            }
        }

        // Nếu va chạm với cả tile và entity, ưu tiên trượt theo hướng thoát
        if (collidesWithTile && collidesWithEntity)
        {
            pushStrength = 2.0f;  // cần tăng lực đẩy khi bị kẹt quá nhiều
            pushDirection.normalize();

            // Kiểm tra hướng thoát khả thi (trái hoặc lên)
            ax::Vec2 leftPos = currentPos + ax::Vec2(-pushStrength, 0);
            ax::Vec2 upPos   = currentPos + ax::Vec2(0, pushStrength);
            bool canMoveLeft = !isCollidingWithTileMap(entity, leftPos);
            bool canMoveUp   = !isCollidingWithTileMap(entity, upPos);

            if (canMoveLeft || canMoveUp)
            {
                ax::Vec2 escapeDir(0, 0);  // ĐN hướng thoát
                if (canMoveLeft)
                    escapeDir.x = -1.0f;  // Ưu tiên thoát trái nếu trống
                if (canMoveUp && !canMoveLeft)
                    escapeDir.y = 1.0f;  // Sau đó là thoát lên
                escapeDir.normalize();
                adjustedPos = currentPos + escapeDir * pushStrength;

                // Kiểm tra lại va chạm entity sau khi đẩy
                bool stillCollides = false;  // Đặt một biến kiểm tra còn va chạm không
                // Kiểm tra va chạm với các entity tại vị trí mới
                auto nearbyAdjusted = spatialGrid.getNearbyEntities(adjustedPos,1);
                for (Entity other : nearbyAdjusted)
                {
                    if (other != entity && identityMgr.getComponent(other) &&
                        (identityMgr.getComponent(other)->type == "enemy" ||
                         identityMgr.getComponent(other)->type == "boss") &&
                        checkCollision(entity, other))
                    {
                        stillCollides = true;
                        break;
                    }
                }
                // Nếu không còn va chạm với tile và entity, trả về adjustedPos
                if (!isCollidingWithTileMap(entity, adjustedPos) && !stillCollides)
                    return adjustedPos;
            }
        }  // Nếu chỉ va chạm entity, không va chạm tile (chỉ cần đẩy)
        else if (collidesWithEntity && !collidesWithTile)
        {
            pushDirection.normalize();
            adjustedPos = newPos + pushDirection * pushStrength;
            if (!isCollidingWithTileMap(entity, adjustedPos))
                return adjustedPos;
        }  // Nếu chỉ va chạm tile, không va chạm entity (di chuyển theo trục)
        else if (collidesWithTile && !collidesWithEntity)
        {
            ax::Vec2 testPosX = currentPos + ax::Vec2(moveStep.x, 0);
            ax::Vec2 testPosY = currentPos + ax::Vec2(0, moveStep.y);

            if (!isCollidingWithTileMap(entity, testPosX))
                return testPosX;
            if (!isCollidingWithTileMap(entity, testPosY))
                return testPosY;
            return currentPos;
        }  // Không va chạm di chuyển bình thường
        else if (!collidesWithTile && !collidesWithEntity)
        {
            return newPos;
        }

        // Nếu vẫn kẹt, trả về vị trí đẩy cuối cùng nếu hợp lệ
        if (!isCollidingWithTileMap(entity, adjustedPos))
            return adjustedPos;
        return currentPos;
    }

    // Projectile: Overlay tất cả, chặn bởi screen bounds
    // if (type == "projectile") // Test
    //{
    //    ax::Vec2 screenSize(720, 1280);
    //    // Nảy lại với vector đối xứng ?? khi va chạm vào screen
    //}

    // Player: Chặn bởi tile, overlay với mọi thứ khác
    if (type == "player")
    {
        if (!isCollidingWithTileMap(entity, newPos))
            return newPos;

        ax::Vec2 testPosX = currentPos + ax::Vec2(moveStep.x, 0);
        ax::Vec2 testPosY = currentPos + ax::Vec2(0, moveStep.y);

        if (!isCollidingWithTileMap(entity, testPosX))
            return testPosX;  // Di chuyển được theo X
        if (!isCollidingWithTileMap(entity, testPosY))
            return testPosY;  // Di chuyển được theo Y
        return currentPos;
    }

    return newPos;  // Không va chạm
}

// Kiểm tra va chạm giữa 2 entity dựa trên hitbox
bool CollisionSystem::checkCollision(Entity e1, Entity e2)
{
    auto transform1 = transformMgr.getComponent(e1);
    auto transform2 = transformMgr.getComponent(e2);
    auto hitbox1    = hitboxMgr.getComponent(e1);
    auto hitbox2    = hitboxMgr.getComponent(e2);

    if (!transform1 || !transform2 || !hitbox1 || !hitbox2)
        return false; // Không va chạm nếu thiếu component

    if (hitbox1->size == ax::Size(0, 0) || hitbox1->size == ax::Size(0, 0))
        return false; //Không va chạm nếu hitbox = 0

    // Tính toán hình chữ nhật hitbox cho cả 2 entity
    ax::Vec2 pos1(transform1->x, transform1->y);
    ax::Vec2 pos2(transform2->x, transform2->y);

    ax::Rect rect1(pos1.x - hitbox1->size.width / 2, pos1.y - hitbox1->size.height / 2, hitbox1->size.width,
               hitbox1->size.height);
    ax::Rect rect2(pos2.x - hitbox2->size.width / 2, pos2.y - hitbox2->size.height / 2, hitbox2->size.width,
               hitbox2->size.height);

    return rect1.intersectsRect(rect2);
    // Hàm kiểm tra giao nhau của axmol: Trả về true nếu 2 hình chữ nhật giao nhau
}

// Kiểm tra va chạm giữa entity và tile map
bool CollisionSystem::isCollidingWithTileMap(Entity entity, const ax::Vec2& position)
{
    auto mapSystem = SystemManager::getInstance()->getSystem<MapSystem>();
    if (!mapSystem)
        return false;

    auto hitbox = hitboxMgr.getComponent(entity);
    if (!hitbox)
        return false;

    // Tính toán 4 góc của hitbox để kiểm tra va chạm
    ax::Vec2 points[4] = {position + ax::Vec2(-hitbox->size.width / 2, -hitbox->size.height / 2),
                          position + ax::Vec2(hitbox->size.width / 2, -hitbox->size.height / 2),
                          position + ax::Vec2(-hitbox->size.width / 2, hitbox->size.height / 2),
                          position + ax::Vec2(hitbox->size.width / 2, hitbox->size.height / 2)};

    for (const ax::Vec2& point : points)
    {
        // Lấy tile map của chunk chứa vị trí
        ax::TMXTiledMap* tiledMap = getChunkTileMap(point);
        if (!tiledMap)
            continue;

        // Lấy layer va chạm từ tile map
        auto collisionLayer = tiledMap->getLayer("CollisionLayer");
        if (!collisionLayer || !collisionLayer->getTiles())
            continue;

        // Chuyển đổi tọa độ pixel sang tọa độ tile
        ax::Vec2 tileCoord = convertToTileCoord(point, tiledMap);
        ax::Size mapSize   = tiledMap->getMapSize();
        // Kiểm tra xem tile tại vị trí đó có phải là tile va chạm và nằm trong map không
        if (tileCoord.x >= 0 && tileCoord.x < mapSize.width && tileCoord.y >= 0 && tileCoord.y < mapSize.height &&
            collisionLayer->getTileGIDAt(tileCoord) != 0)
        { // Lấy GID của tile nếu khác 0 là có va chạm
            return true;
        }
    }
    return false;
}


ax::Vec2 CollisionSystem::convertToTileCoord(const ax::Vec2& position, ax::TMXTiledMap* tiledMap)
{
    ax::Vec2 mapPos = tiledMap->getPosition();
    ax::Size tileSize = tiledMap->getTileSize();
    ax::Size mapSize  = tiledMap->getMapSize();

    float x   = (position.x - mapPos.x) / tileSize.width;
    float y   = (position.y - mapPos.y) / tileSize.height;
    int tileX = std::floor(x);
    int tileY = std::floor(mapSize.height - y); // Đảo ngược Y để khớp với TMX do tileMap có gốc tọa độ (0,0) nằm ở góc trên bên trái

    return ax::Vec2(tileX, tileY);
}

ax::TMXTiledMap* CollisionSystem::getChunkTileMap(const ax::Vec2& position)
{
    auto mapSystem = SystemManager::getInstance()->getSystem<MapSystem>();
    if (!mapSystem)
        return nullptr;

    // Tính chunk từ vị trí thế giới
    int chunkX = std::floor(position.x / mapSystem->getChunkSize().x);
    int chunkY = std::floor((mapSystem->getGridSize().y * mapSystem->getChunkSize().y - position.y - 1) /
                            mapSystem->getChunkSize().y); // Lật Y
    // Kiểm tra xem chunk có nằm trong giới hạn không
    if (chunkX >= 0 && chunkX < mapSystem->getGridSize().x && chunkY >= 0 && chunkY < mapSystem->getGridSize().y)
    {
        const auto& grid = mapSystem->getGrid();
        return grid[chunkY][chunkX].tiledMap;// Trả về tile map của chunk
    }
    return nullptr;// Trả về nullptr nếu ngoài giới hạn
}

// Spatial Grid khởi tạo
void CollisionSystem::SpatialGrid::init(const ax::Vec2& worldSize)
{
    // init SpatialGrid với kích thước lưới
    gridSize.x = std::ceil(worldSize.x / cellSize.x);
    gridSize.y = std::ceil(worldSize.y / cellSize.y);
    // Cấp bộ nhớ cho lưới với gridSize.y hàng và gridSize.x phần tử mỗi hàng
    cells.resize(gridSize.y, std::vector<std::vector<Entity>>(gridSize.x));
}

// Xóa toàn bộ Entity trong lưới
void CollisionSystem::SpatialGrid::clear()
{
    for (auto& row : cells)
        for (auto& cell : row)
            cell.clear();
}

// Thêm entity vào lưới dựa trên vị trí entity
void CollisionSystem::SpatialGrid::insert(Entity entity, const ax::Vec2& pos)
{
    int cellX = std::floor(pos.x / cellSize.x); //Tính vị trí X trong lưới
    int cellY = std::floor(pos.y / cellSize.y); //Tính vị trí Y trong lưới
    //Kiểm tra có nằm trong giới hạn lưới không và thêm vào ô entity đó
    if (cellX >= 0 && cellX < gridSize.x && cellY >= 0 && cellY < gridSize.y)
        cells[cellY][cellX].push_back(entity);
}

// Lấy danh sách các entity gần vị trí (trong ô và radius ô xung quanh của lưới)
std::vector<Entity> CollisionSystem::SpatialGrid::getNearbyEntities(const ax::Vec2& pos, int radius)
{
    std::vector<Entity> nearby;
    int cellX = std::floor(pos.x / cellSize.x);  // Tính vị trí X trong lưới
    int cellY = std::floor(pos.y / cellSize.y);  // Tính vị trí Y trong lưới

    // Duyệt qua ô hiện tại và ô xung quanh dựa trên radius trong lưới
    for (int dy = -radius; dy <= radius; dy++)
    {
        for (int dx = -radius; dx <= radius; dx++)
        {
            int nx = cellX + dx;
            int ny = cellY + dy;
            if (nx >= 0 && nx < gridSize.x && ny >= 0 && ny < gridSize.y)
            {
                // Thêm tất cả entity trong ô vào danh sách
                nearby.insert(nearby.end(), cells[ny][nx].begin(), cells[ny][nx].end());
            }
        }
    }
    return nearby;  // Trả về danh sách entity gần vị trí
}

void CollisionSystem::weaponCollisionCheck()
{
    auto weaponPool = SystemManager::getInstance()->getSystem<WeaponSystem>()->getWeaponEntities();
    for (auto& weapon : weaponPool)
    {
        auto weaponPos    = ax::Vec2(transformMgr.getComponent(weapon)->x, transformMgr.getComponent(weapon)->y);
        auto weaponHitbox = hitboxMgr.getComponent(weapon);
        int weaponMaxSize = std::max(static_cast<int>(weaponHitbox->defaultSize.width),
                                     static_cast<int>(weaponHitbox->defaultSize.height));

        // Dùng radius đủ lớn để bao quát cả boss (tối đa của entity là 64)
        int maxEntitySize = 64;  // Kích thước tối đa của hitbox entity (dành cho boss)
        int baseRadius = std::max(1, (weaponMaxSize + maxEntitySize) / 2 / 16);  // Tổng bán kính tối đa theo gridSize

        auto entitiesNearby = spatialGrid.getNearbyEntities(weaponPos, baseRadius);

        for (auto& entity : entitiesNearby)
        {
            auto entityType = identityMgr.getComponent(entity)->type;
            if (Utils::in(entityType, "enemy", "boss"))
            {
                auto entityPos = ax::Vec2(transformMgr.getComponent(entity)->x, transformMgr.getComponent(entity)->y);
                auto entityHitbox = hitboxMgr.getComponent(entity);
                int entityMaxSize = std::max(static_cast<int>(entityHitbox->defaultSize.width),
                                             static_cast<int>(entityHitbox->defaultSize.height));

                // Tính khoảng cách thực tế giữa hai tâm
                float distance          = weaponPos.distance(entityPos);
                float collisionDistance = (weaponMaxSize + entityMaxSize) / 2.0f;  // Tổng bán kính

                if (distance <= collisionDistance && checkCollision(weapon, entity))
                {
                    onWeaponCollision(weapon, entity);
                }
            }
        }
    }

    auto tempWeponPool = SystemManager::getInstance()->getSystem<WeaponSystem>()->getTempWeaponEntities();
    for (auto& weapon : tempWeponPool)
    {
        auto weaponPos    = ax::Vec2(transformMgr.getComponent(weapon)->x, transformMgr.getComponent(weapon)->y);
        auto entitiesNearby = spatialGrid.getNearbyEntities(weaponPos, 1);

        for (auto& entity : entitiesNearby)
        {
            auto entityType = identityMgr.getComponent(entity)->type;
            if (Utils::in(entityType, "enemy", "boss") && checkCollision(weapon, entity))
            {
                if (identityMgr.getComponent(weapon)->name == "kunai")
                {
                    onWeaponCollision(weapon, entity);
                    SystemManager::getInstance()->getSystem<CleanupSystem>()->destroyItem(weapon);
                    break;
                }
                else
                {
                    onWeaponCollision(weapon, entity);
                }
            }
        }
    }
}
