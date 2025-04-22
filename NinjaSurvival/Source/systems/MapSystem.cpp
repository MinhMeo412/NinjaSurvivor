#include "MapSystem.h"
#include "SystemManager.h"
#include "CameraSystem.h"
#include "GameData.h"

using namespace ax;

MapSystem::MapSystem() {}

void MapSystem::init()
{
    auto gameData               = GameData::getInstance();
    std::string selectedMapName = gameData->getSelectedMap();
    if (selectedMapName.empty())
    {
        // AXLOG("MapSystem: No selected map in GameData");
        return;
    }

    auto systemManager = SystemManager::getInstance();
    parentScene        = systemManager->getCurrentScene();

    // Lấy data map
    const auto& map = gameData->getMaps().at(selectedMapName);
    // Xác định kích thước grid qua data map
    gridSize = Vec2(map.mapWidth, map.mapHeight);
    chunkSize = Vec2(2048, 2048);  // 128 tile * 16 pixel = 2048x2048 pixel  //Sử dụng 256x256 nếu dùng LargeMap để test
    // Cấp bộ nhớ cho grid với gridSize.y hàng và gridSize.x phần tử mỗi hàng
    grid.resize(gridSize.y, std::vector<Chunk>(gridSize.x));

    // Lấy tên player
    std::string selectedCharacterName = GameData::getInstance()->getSelectedCharacter();
    // truy cập template (có chứa dữ liệu vị trí player spawn)
    const auto& playerTemplate = gameData->getEntityTemplates().at("player").at(selectedCharacterName);  // type & name
    // Lấy vị trí spawn mặc định của player được lưu trong data template
    ax::Vec2 spawnPos = ax::Vec2(playerTemplate.transform->x, playerTemplate.transform->y);

    // Tính chunk và góc phần tư ban đầu
    ax::Vec2 currentChunk = getChunkCoordFromPosition(spawnPos);
    float chunkXWorldPos  = currentChunk.x * chunkSize.x;
    float chunkYWorldPos  = (gridSize.y - currentChunk.y - 1) * chunkSize.y;
    ax::Vec2 localPos     = ax::Vec2(spawnPos.x - chunkXWorldPos, spawnPos.y - chunkYWorldPos);

    bool inUpperHalf = localPos.y > chunkSize.y / 2;  // Kiểm tra xem có ở nửa trên không
    bool inRightHalf = localPos.x > chunkSize.x / 2;  // Kiểm tra xem có ở nửa bên phải không

    std::string currentQuadrant =
        (inUpperHalf && inRightHalf) ? "UR" : (inUpperHalf ? "UL" : (inRightHalf ? "LR" : "LL"));

    // Load ngay lập tức các chunk cần thiết cho visibility lần đầu
    loadChunk(currentChunk);  // Chunk hiện tại

    drawCollisionOutline(grid[currentChunk.y][currentChunk.x]);  // Vẽ viền ngay sau khi load để xem collision

    std::vector<Vec2> quadrantChunks = getQuadrantChunks(currentQuadrant, currentChunk);
    for (const Vec2& pos : quadrantChunks)
    {
        if (pos.x >= 0 && pos.x < gridSize.x && pos.y >= 0 && pos.y < gridSize.y)
        {
            loadChunk(pos);
            drawCollisionOutline(grid[pos.y][pos.x]);  // Vẽ viền ngay sau khi load để xem collision
        }
    }

    // Cập nhật visibility lần đầu tiên
    updateChunkVisibility(spawnPos, currentChunk, currentQuadrant);
    lastPlayerChunk = currentChunk;
    lastQuadrant    = currentQuadrant;

    // Thêm các chunk còn lại vào queue để load bất đồng bộ
    for (int y = 0; y < gridSize.y; y++)
    {
        for (int x = 0; x < gridSize.x; x++)
        {
            Vec2 pos(x, y);
            if (!grid[y][x].tiledMap)      // Nếu chưa load (chưa có tiledMap)
                chunkLoadQueue.push(pos);  // Push vào hàng đợi
        }
    }
    scheduleChunkLoading();
}

void MapSystem::scheduleChunkLoading()
{
    if (!isLoadingChunks && !chunkLoadQueue.empty())
    {
        isLoadingChunks = true;
        Director::getInstance()->getScheduler()->schedule([this](float dt) { loadNextChunk(dt); }, this, 0.0f, false,
                                                          "chunk_loader");
    }
}

void MapSystem::loadNextChunk(float dt)
{
    if (chunkLoadQueue.empty())
    {
        isLoadingChunks = false;
        Director::getInstance()->getScheduler()->unschedule("chunk_loader", this);
        // AXLOG("MapSystem: All chunks loaded asynchronously");
        return;
    }

    // Load 2 chunk mỗi frame với i < 2
    for (int i = 0; i < 1 && !chunkLoadQueue.empty(); i++)
    {
        Vec2 chunkPos = chunkLoadQueue.front();
        chunkLoadQueue.pop();
        if (!grid[chunkPos.y][chunkPos.x].tiledMap)  // Chỉ load nếu chưa có tiledMap
            loadChunk(chunkPos);
    }
}

void MapSystem::loadChunk(const Vec2& chunkPos)
{
    auto gameData               = GameData::getInstance();
    std::string selectedMapName = gameData->getSelectedMap();
    const auto& map             = gameData->getMaps().at(selectedMapName);

    Chunk& chunk  = grid[chunkPos.y][chunkPos.x];
    chunk.gridPos = chunkPos;

    std::string tmxFile;
    if (chunkPos.x == 0 && chunkPos.y == 0)
        tmxFile = getRandomTMXFile(map.upLeftCornerTMXFile);
    else if (chunkPos.x == gridSize.x - 1 && chunkPos.y == 0)
        tmxFile = getRandomTMXFile(map.upRightCornerTMXFile);
    else if (chunkPos.x == 0 && chunkPos.y == gridSize.y - 1)
        tmxFile = getRandomTMXFile(map.downLeftCornerTMXFile);
    else if (chunkPos.x == gridSize.x - 1 && chunkPos.y == gridSize.y - 1)
        tmxFile = getRandomTMXFile(map.downRightCornerTMXFile);
    else if (chunkPos.y == 0)
        tmxFile = getRandomTMXFile(map.upRearTMXFile);
    else if (chunkPos.y == gridSize.y - 1)
        tmxFile = getRandomTMXFile(map.downRearTMXFile);
    else if (chunkPos.x == 0)
        tmxFile = getRandomTMXFile(map.leftRearTMXFile);
    else if (chunkPos.x == gridSize.x - 1)
        tmxFile = getRandomTMXFile(map.rightRearTMXFile);
    else
        tmxFile = getRandomTMXFile(map.middleTMXFile);

    if (tmxFile.empty())
    {
        // AXLOG("MapSystem: No valid TMX file for chunk (%d, %d)", (int)chunkPos.x, (int)chunkPos.y);
        return;
    }

    chunk.tiledMap = TMXTiledMap::create(tmxFile);
    if (!chunk.tiledMap)
    {
        drawCollisionOutline(chunk);
        // AXLOG("MapSystem: Failed to load TMX file %s", tmxFile.c_str());
        return;
    }

    Vec2 worldPos = Vec2(chunkPos.x * chunkSize.x, (gridSize.y - 1 - chunkPos.y) * chunkSize.y);
    chunk.tiledMap->setPosition(worldPos);
    chunk.tiledMap->setVisible(false);  // Mặc định ẩn
    parentScene->addChild(chunk.tiledMap, 0);

    auto overlayLayer = chunk.tiledMap->getLayer("OverlayLayer");
    if (overlayLayer)
        overlayLayer->setGlobalZOrder(1);

    // Vẽ viền ngay sau khi load chunk
    drawCollisionOutline(chunk);

    // AXLOG("Loaded chunk (%d, %d)", (int)chunkPos.x, (int)chunkPos.y);
}

std::string MapSystem::getRandomTMXFile(const std::vector<std::string>& tmxFiles)
{
    if (tmxFiles.empty())
        return "";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, tmxFiles.size() - 1);
    return tmxFiles[dis(gen)];
}

void MapSystem::update(float dt) {}

void MapSystem::onPlayerPositionChanged(const Vec2& playerPos)
{
    Vec2 currentChunk = getChunkCoordFromPosition(playerPos);

    // Tính góc phần tư hiện tại để kiểm tra sự thay đổi
    float chunkXWorldPos = currentChunk.x * chunkSize.x;
    float chunkYWorldPos = (gridSize.y - currentChunk.y - 1) * chunkSize.y;
    Vec2 localPos        = Vec2(playerPos.x - chunkXWorldPos, playerPos.y - chunkYWorldPos);

    bool inUpperHalf = localPos.y > chunkSize.y / 2;
    bool inRightHalf = localPos.x > chunkSize.x / 2;

    std::string currentQuadrant =
        (inUpperHalf && inRightHalf) ? "UR" : (inUpperHalf ? "UL" : (inRightHalf ? "LR" : "LL"));

    // AXLOG("Current quadrant: %s", currentQuadrant.c_str());

    // Cập nhật visibility khi chunk thay đổi HOẶC góc phần tư thay đổi
    if (currentChunk != lastPlayerChunk || currentQuadrant != lastQuadrant)
    {
        updateChunkVisibility(playerPos, currentChunk, currentQuadrant);
    }
}

ax::Vec2 MapSystem::getChunkCoordFromPosition(const Vec2& pos)
{
    // Tính chunk theo tọa độ thế giới
    int chunkX = std::max(0, std::min(static_cast<int>(floor(pos.x / chunkSize.x)), static_cast<int>(gridSize.x) - 1));
    int chunkY = std::max(
        0, std::min(static_cast<int>(floor((gridSize.y * chunkSize.y - pos.y - 1) / chunkSize.y)),
                    static_cast<int>(gridSize.y) - 1));  // Lật Y //Sử dụng min max tránh vượt grid, ép kiểu int
    // AXLOG("Player pos: (%f, %f), Chunk coord: (%d, %d)", pos.x, pos.y, chunkX, chunkY);
    return Vec2(chunkX, chunkY);
}

void MapSystem::updateChunkVisibility(const Vec2& playerPos,
                                      const Vec2& currentChunk,
                                      const std::string& currentQuadrant)
{
    // Đảm bảo chunk hiện tại đã load
    if (!grid[currentChunk.y][currentChunk.x].tiledMap)
        loadChunk(currentChunk);

    // Luôn giữ chunk hiện tại visible
    // if (currentChunk.x >= 0 && currentChunk.x < gridSize.x && currentChunk.y >= 0 && currentChunk.y < gridSize.y)
    //{
    //    Chunk& current    = grid[currentChunk.y][currentChunk.x];
    //    current.isVisible = true;
    //    current.tiledMap->setVisible(true);
    //}

    Chunk& current    = grid[currentChunk.y][currentChunk.x];
    current.isVisible = true;
    current.tiledMap->setVisible(true);
    if (!current.tiledMap->getChildByTag(999))  // Kiểm tra và vẽ lại viền nếu cần
        drawCollisionOutline(current);

    // Chỉ cập nhật khi chunk hoặc góc phần tư thay đổi
    if (currentChunk != lastPlayerChunk || currentQuadrant != lastQuadrant)
    {
        // Tắt chunk của góc phần tư cũ (nếu có)
        if (!lastQuadrant.empty())
        {
            std::vector<Vec2> oldChunks = getQuadrantChunks(lastQuadrant, lastPlayerChunk);
            for (const Vec2& pos : oldChunks)
            {
                if (pos.x >= 0 && pos.x < gridSize.x && pos.y >= 0 && pos.y < gridSize.y)
                {
                    // Không tắt chunk hiện tại
                    if (pos != currentChunk)
                    {
                        Chunk& chunk    = grid[pos.y][pos.x];
                        chunk.isVisible = false;
                        if (chunk.tiledMap)
                            chunk.tiledMap->setVisible(false);
                    }
                }
            }
        }
    }

    // Bật chunk của góc phần tư mới
    std::vector<Vec2> newChunks = getQuadrantChunks(currentQuadrant, currentChunk);
    for (const Vec2& pos : newChunks)
    {
        if (pos.x >= 0 && pos.x < gridSize.x && pos.y >= 0 && pos.y < gridSize.y)
        {
            if (!grid[pos.y][pos.x].tiledMap)
            {
                loadChunk(pos);  // Load ngay nếu chưa có
            }

            Chunk& chunk = grid[pos.y][pos.x];
            // Chỉ set true nếu chưa visible (tránh lặp)
            if (!chunk.isVisible)
            {
                chunk.isVisible = true;
                chunk.tiledMap->setVisible(true);

                // Kiểm tra và vẽ lại viền nếu chưa có DrawNode
                if (!chunk.tiledMap->getChildByTag(999))
                    drawCollisionOutline(chunk);
            }
        }
    }

    // Thêm các chunk mới vào queue nếu chưa load
    for (int y = 0; y < gridSize.y; y++)
    {
        for (int x = 0; x < gridSize.x; x++)
        {
            Vec2 pos(x, y);
            if (!grid[y][x].tiledMap && std::find(newChunks.begin(), newChunks.end(), pos) == newChunks.end() &&
                pos != currentChunk)
            {
                chunkLoadQueue.push(pos);
            }
        }
    }
    scheduleChunkLoading();

    lastQuadrant    = currentQuadrant;
    lastPlayerChunk = currentChunk;
}

std::vector<Vec2> MapSystem::getQuadrantChunks(const std::string& quadrant, const Vec2& chunkPos)
{
    std::vector<Vec2> chunks;
    if (quadrant == "UR")
    {
        chunks.push_back(Vec2(chunkPos.x + 1, chunkPos.y));      // Phải
        chunks.push_back(Vec2(chunkPos.x, chunkPos.y - 1));      // Trên
        chunks.push_back(Vec2(chunkPos.x + 1, chunkPos.y - 1));  // Chéo trên-phải
    }
    else if (quadrant == "UL")
    {
        chunks.push_back(Vec2(chunkPos.x - 1, chunkPos.y));      // Trái
        chunks.push_back(Vec2(chunkPos.x, chunkPos.y - 1));      // Trên
        chunks.push_back(Vec2(chunkPos.x - 1, chunkPos.y - 1));  // Chéo trên-trái
    }
    else if (quadrant == "LR")
    {
        chunks.push_back(Vec2(chunkPos.x + 1, chunkPos.y));      // Phải
        chunks.push_back(Vec2(chunkPos.x, chunkPos.y + 1));      // Dưới
        chunks.push_back(Vec2(chunkPos.x + 1, chunkPos.y + 1));  // Chéo dưới-phải
    }
    else if (quadrant == "LL")
    {
        chunks.push_back(Vec2(chunkPos.x - 1, chunkPos.y));      // Trái
        chunks.push_back(Vec2(chunkPos.x, chunkPos.y + 1));      // Dưới
        chunks.push_back(Vec2(chunkPos.x - 1, chunkPos.y + 1));  // Chéo dưới-trái
    }
    return chunks;
}

void MapSystem::drawCollisionOutline(Chunk& chunk)
{
    return;

    if (!chunk.tiledMap)
        return;  // Không có tiledMap thì bỏ qua

    // Nếu đã có DrawNode với tag 999 thì không vẽ lại
    if (chunk.tiledMap->getChildByTag(999))
        return;

    auto collisionLayer = chunk.tiledMap->getLayer("CollisionLayer");
    if (collisionLayer)
    {
        auto drawNode  = DrawNode::create();
        Size layerSize = collisionLayer->getLayerSize();
        Size tileSize  = chunk.tiledMap->getTileSize();

        for (int x = 0; x < layerSize.width; x++)
        {
            for (int y = 0; y < layerSize.height; y++)
            {
                if (collisionLayer->getTileAt(Vec2(x, y)))
                {
                    Vec2 tilePos = Vec2(x * tileSize.width, (layerSize.height - y - 1) * tileSize.height);
                    drawNode->drawRect(tilePos, tilePos + Vec2(tileSize.width, tileSize.height), Color4F::RED);
                }
            }
        }
        drawNode->setTag(999);
        chunk.tiledMap->addChild(drawNode, 10);
        // AXLOG("Drew collision outline for chunk (%d, %d)", (int)chunk.gridPos.x, (int)chunk.gridPos.y);
    }
}
