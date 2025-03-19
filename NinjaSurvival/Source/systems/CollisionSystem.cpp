#include "CollisionSystem.h"
#include "SpawnSystem.h"
#include "MapSystem.h"

#include "SystemManager.h"


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

    // Đặt callback logic khi va chạm xảy ra
    setCollisionCallback([](Entity e1, Entity e2)
        {
            //Thông báo
            AXLOG("Collision between %u and %u", e1, e2);
            //Xử lý logic va chạm
            //Ví dụ: 1+1 = 2
        });
}

// cập nhật hệ thống va chạm, gọi mỗi frame
void CollisionSystem::update(float dt)
{
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
    auto entities = entityManager.getActiveEntities();
    for (Entity entity : entities)
    {
        //Kiểm tra có va chạm không
        if (entity != player && checkCollision(player, entity))
        {
            if (onCollision) // Kiểm tra biến onCollision đã được gán callback hay chưa
                onCollision(player, entity); // Gọi callback
        }
    }
}

// Hàm xử lý vị trí mới của entity khi di chuyểnzxc
ax::Vec2 CollisionSystem::resolvePosition(Entity entity, const ax::Vec2& newPos)
{
    auto identity = identityMgr.getComponent(entity);
    if (!identity)
        return newPos;// Trả về vị trí mới nếu không có identity

    std::string type = identity->type;
    // Lấy vị trí hiện tại của entity
    ax::Vec2 currentPos  = transformMgr.getComponent(entity)
                              ? ax::Vec2(transformMgr.getComponent(entity)->x, transformMgr.getComponent(entity)->y)
                           : newPos;
    ax::Vec2 moveStep   = newPos - currentPos; // Tính bước di chuyển

    // Enemy/Boss: Không overlay với enemy/boss, chặn bởi collision tile
    if (type == "enemy" || type == "boss")
    {
        //Kiểm tra va chạm với tile và entity khác
        bool collidesWithTile   = isCollidingWithTileMap(entity, newPos);
        bool collidesWithEntity = false;
        //Lấy các entity ở gần trong lưới
        auto nearby             = spatialGrid.getNearbyEntities(newPos);
        for (Entity other : nearby)
        {
            if (other != entity && identityMgr.getComponent(other))
            {
                std::string otherType = identityMgr.getComponent(other)->type;
                if ((otherType == "enemy" || otherType == "boss") && checkCollision(entity, other))
                {
                    collidesWithEntity = true;
                    break;
                }
            }
        }

        if (!collidesWithTile && !collidesWithEntity)
            return newPos; // Trả về vị trí mới nếu không có va chạm

        //Thử di chuyển theo trục X,Y
        ax::Vec2 testPosX = currentPos + ax::Vec2(moveStep.x, 0);
        ax::Vec2 testPosY = currentPos + ax::Vec2(0, moveStep.y);

        bool canMoveXTile   = !isCollidingWithTileMap(entity, testPosX);
        bool canMoveXEntity = true;
        if (canMoveXTile)
        {
            auto nearbyX = spatialGrid.getNearbyEntities(testPosX);
            for (Entity other : nearbyX)
            {
                if (other != entity && identityMgr.getComponent(other))
                {
                    std::string otherType = identityMgr.getComponent(other)->type;
                    if ((otherType == "enemy" || otherType == "boss") && checkCollision(entity, other))
                    {
                        canMoveXEntity = false;
                        break;
                    }
                }
            }
        }

        bool canMoveYTile   = !isCollidingWithTileMap(entity, testPosY);
        bool canMoveYEntity = true;
        if (canMoveYTile)
        {
            auto nearbyY = spatialGrid.getNearbyEntities(testPosY);
            for (Entity other : nearbyY)
            {
                if (other != entity && identityMgr.getComponent(other))
                {
                    std::string otherType = identityMgr.getComponent(other)->type;
                    if ((otherType == "enemy" || otherType == "boss") && checkCollision(entity, other))
                    {
                        canMoveYEntity = false;
                        break;
                    }
                }
            }
        }

        if (canMoveXTile && canMoveXEntity)
            return testPosX; // Di chuyển được theo X
        if (canMoveYTile && canMoveYEntity)
            return testPosY; // Di chuyển được theo y
        return currentPos;
    }

    // Projectile: Overlay tất cả, chặn bởi screen bounds
    //if (type == "projectile") // Test 
    //{
    //    ax::Vec2 screenSize(720, 1280);
    //    // Nảy lại với vector đối xứng ?? khi va chạm vào screen
    //}

    // Item: Overlay/tunneling tất cả, không chặn bởi tile
    if (type == "item")
    {
        return newPos;  // Không kiểm tra gì cả
    }

    // Player: Chặn bởi tile, overlay với mọi thứ khác
    if (type == "player")
    {
        if (!isCollidingWithTileMap(entity, newPos))
            return newPos;

        ax::Vec2 testPosX = currentPos + ax::Vec2(moveStep.x, 0);
        ax::Vec2 testPosY = currentPos + ax::Vec2(0, moveStep.y);

        if (!isCollidingWithTileMap(entity, testPosX))
            return testPosX; // Di chuyển được theo X
        if (!isCollidingWithTileMap(entity, testPosY))
            return testPosY; // Di chuyển được theo Y
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
        // Kiểm tra xem tile tại vị trí đó có phải là tile va chạm không
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

// Lấy danh sách các entity gần vị trí (trong ô và 8 ô xung quanh của lưới)
std::vector<Entity> CollisionSystem::SpatialGrid::getNearbyEntities(const ax::Vec2& pos)
{
    std::vector<Entity> nearby;
    int cellX = std::floor(pos.x / cellSize.x); //Tính vị trí X trong lưới
    int cellY = std::floor(pos.y / cellSize.y); //Tính vị trí Y trong lưới

    // Duyệt qua 9 ô (ô hiện tại và 8 ô xung quanh) trong lưới
    for (int dy = -1; dy <= 1; dy++)
    {
        for (int dx = -1; dx <= 1; dx++)
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
    return nearby; // Trả về danh sách entity gần vị trí
}
