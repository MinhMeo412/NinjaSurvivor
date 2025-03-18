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
        AXLOG("MapSystem: No selected map in GameData");
        return;
    }

    auto systemManager = SystemManager::getInstance();
    parentScene        = systemManager->getCurrentScene();

    //Lấy data map
    const auto& map = gameData->getMaps().at(selectedMapName);

    //Xác định kích thước grid qua data map
    gridSize = Vec2(map.mapWidth, map.mapHeight);

    chunkSize = Vec2(256, 256); // 128 tile * 16 pixel = 2048x2048 pixel
    //Cấp bộ nhớ cho grid với gridSize.y hàng và gridSize.x phần tử mỗi hàng
    grid.resize(gridSize.y, std::vector<Chunk>(gridSize.x));

    //Tạo chunks
    loadMapChunks();

    std::string selectedCharacterName = GameData::getInstance()->getSelectedCharacter();
    //truy cập template (có chứa dữ liệu vị trí player spawn)
    const auto& playerTemplate = gameData->getEntityTemplates().at("player").at(selectedCharacterName);  // type & name
    //Lấy vị trí spawn mặc định của player được lưu trong data template
    ax::Vec2 spawnPos = ax::Vec2(playerTemplate.transform->x, playerTemplate.transform->y);

    // Tính chunk và góc phần tư ban đầu
    ax::Vec2 currentChunk = getChunkCoordFromPosition(spawnPos);
    float chunkXWorldPos  = currentChunk.x * chunkSize.x;
    float chunkYWorldPos  = (gridSize.y - currentChunk.y - 1) * chunkSize.y;
    ax::Vec2 localPos     = ax::Vec2(spawnPos.x - chunkXWorldPos, spawnPos.y - chunkYWorldPos);

    bool inUpperHalf = localPos.y > chunkSize.y / 2;
    bool inRightHalf = localPos.x > chunkSize.x / 2;

    std::string currentQuadrant;
    if (inUpperHalf && inRightHalf)
        currentQuadrant = "UR";
    else if (inUpperHalf && !inRightHalf)
        currentQuadrant = "UL";
    else if (!inUpperHalf && inRightHalf)
        currentQuadrant = "LR";
    else
        currentQuadrant = "LL";

    // Cập nhật visibility lần đầu tiên
    updateChunkVisibility(spawnPos, currentChunk, currentQuadrant);
}

void MapSystem::loadMapChunks()
{
    auto gameData               = GameData::getInstance();
    std::string selectedMapName = gameData->getSelectedMap();
    const auto& map = gameData->getMaps().at(selectedMapName);

    //Duyệt qua từng vị trí chunk trong grid
    for (int y = 0; y < gridSize.y; y++)
    {
        for (int x = 0; x < gridSize.x; x++)
        {
            //Truy cập struct chunk tại vị trí x,y
            Chunk& chunk  = grid[y][x];
            //Gán cho giá trị gridPos trong struct = Vec2(x, y)
            chunk.gridPos = Vec2(x, y);

            // Chọn file TMX tương ứng vị trí chunk
            std::string tmxFile;
            if (x == 0 && y == 0)
                tmxFile = getRandomTMXFile(map.upLeftCornerTMXFile);  // Góc trên trái
            else if (x == gridSize.x - 1 && y == 0)
                tmxFile = getRandomTMXFile(map.upRightCornerTMXFile);  // Góc trên phải
            else if (x == 0 && y == gridSize.y - 1)
                tmxFile = getRandomTMXFile(map.downLeftCornerTMXFile);  // Góc dưới trái
            else if (x == gridSize.x - 1 && y == gridSize.y - 1)
                tmxFile = getRandomTMXFile(map.downRightCornerTMXFile);  // Góc dưới phải
            else if (y == 0)
                tmxFile = getRandomTMXFile(map.upRearTMXFile);  // Hàng trên
            else if (y == gridSize.y - 1)
                tmxFile = getRandomTMXFile(map.downRearTMXFile);  // Hàng dưới
            else if (x == 0)
                tmxFile = getRandomTMXFile(map.leftRearTMXFile);  // Cột trái
            else if (x == gridSize.x - 1)
                tmxFile = getRandomTMXFile(map.rightRearTMXFile);  // Cột phải
            else
                tmxFile = getRandomTMXFile(map.middleTMXFile);

            // Kiểm tra nếu không có file hợp lệ
            if (tmxFile.empty())
            {
                AXLOG("MapSystem: No valid TMX file found for chunk (%d, %d)", x, y);
                continue;
            }

            chunk.tiledMap      = TMXTiledMap::create(tmxFile);
            if (!chunk.tiledMap)
            {
                AXLOG("MapSystem: Failed to load TMX file %s", tmxFile.c_str());
                continue;
            }

            Vec2 worldPos = Vec2(x * chunkSize.x, (gridSize.y - 1 - y) * chunkSize.y);
            chunk.tiledMap->setPosition(worldPos);
            chunk.tiledMap->setVisible(false);  // Ẩn mặc định
            parentScene->addChild(chunk.tiledMap,0);

            auto overlayLayer = chunk.tiledMap->getLayer("OverlayLayer");
            overlayLayer->setGlobalZOrder(1);

            // Vẽ viền đỏ cho CollisionLayer
            auto collisionLayer = chunk.tiledMap->getLayer("CollisionLayer");
            auto drawNode1 = DrawNode::create();
            parentScene->addChild(drawNode1,5);
            if (collisionLayer)
            {
                Size layerSize = collisionLayer->getLayerSize();
                Size tileSize  = chunk.tiledMap->getTileSize();
                Vec2 mapPos    = chunk.tiledMap->getPosition();

                for (int x = 0; x < layerSize.width; x++)
                {
                    for (int y = 0; y < layerSize.height; y++)
                    {
                        if (collisionLayer->getTileAt(Vec2(x, y)))
                        {
                            // Tính toán tọa độ thế giới của ô
                            Vec2 tilePos = Vec2(mapPos.x + x * tileSize.width,
                                                mapPos.y + (layerSize.height - y - 1) * tileSize.height);
                            // Vẽ hình chữ nhật viền đỏ
                            drawNode1->drawRect(tilePos,                                          // Góc dưới trái
                                                tilePos + Vec2(tileSize.width, tileSize.height),  // Góc trên phải
                                                Color4F::RED                                      // Màu đỏ
                            );
                        }
                    }
                }
            }
        }
    }
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


void MapSystem::update(float dt)
{

}

void MapSystem::onPlayerPositionChanged(const Vec2& playerPos)
{
    Vec2 currentChunk = getChunkCoordFromPosition(playerPos);

    // Tính góc phần tư hiện tại để kiểm tra sự thay đổi
    float chunkXWorldPos = currentChunk.x * chunkSize.x;
    float chunkYWorldPos = (gridSize.y - currentChunk.y - 1) * chunkSize.y;
    Vec2 localPos        = Vec2(playerPos.x - chunkXWorldPos, playerPos.y - chunkYWorldPos);

    bool inUpperHalf = localPos.y > chunkSize.y / 2;
    bool inRightHalf = localPos.x > chunkSize.x / 2;

    std::string currentQuadrant;
    if (inUpperHalf && inRightHalf)
        currentQuadrant = "UR";
    else if (inUpperHalf && !inRightHalf)
        currentQuadrant = "UL";
    else if (!inUpperHalf && inRightHalf)
        currentQuadrant = "LR";
    else
        currentQuadrant = "LL";

    AXLOG("Current quadrant: %s", currentQuadrant.c_str());

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
    int chunkY = std::max(0, std::min(static_cast<int>(floor((gridSize.y * chunkSize.y - pos.y - 1) / chunkSize.y)),
                                      static_cast<int>(gridSize.y) - 1));  // Lật Y //Sử dụng min max tránh vượt grid, ép kiểu int
    AXLOG("Player pos: (%f, %f), Chunk coord: (%d, %d)", pos.x, pos.y, chunkX, chunkY);
    return Vec2(chunkX, chunkY);
}

void MapSystem::updateChunkVisibility(const Vec2& playerPos,
                                      const Vec2& currentChunk,
                                      const std::string& currentQuadrant)
{
    // Luôn giữ chunk hiện tại visible
    if (currentChunk.x >= 0 && currentChunk.x < gridSize.x && currentChunk.y >= 0 && currentChunk.y < gridSize.y)
    {
        Chunk& chunk    = grid[currentChunk.y][currentChunk.x];
        chunk.isVisible = true;
        chunk.tiledMap->setVisible(true);
    }

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
            Chunk& chunk = grid[pos.y][pos.x];
            // Chỉ set true nếu chưa visible (tránh lặp)
            if (!chunk.isVisible)
            {
                chunk.isVisible = true;
                chunk.tiledMap->setVisible(true);
            }
        }
    }

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
