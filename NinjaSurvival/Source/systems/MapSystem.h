#ifndef __MAP_SYSTEM_H__
#define __MAP_SYSTEM_H__

#include "System.h"
#include "axmol.h"

class MapSystem : public System
{
public:
    MapSystem();      
    void init() override;                   
    void update(float dt) override;

    // Callback để cập nhật visibility khi player đổi chunk hoặc đổi góc phần tư
    void onPlayerPositionChanged(const ax::Vec2& playerPos);
private:        
    ax::Scene* parentScene   = nullptr;

    struct Chunk //Cấu trúc chunk
    {
        ax::TMXTiledMap* tiledMap = nullptr;
        ax::Vec2 gridPos;
        bool isVisible = false;
    };

    std::vector<std::vector<Chunk>> grid; //Grid chứa chunk
    ax::Vec2 chunkSize; //kích thước chunk theo pixel (hiện là 2048x2048
    ax::Vec2 gridSize; //Kích thước grid chunk
    ax::Vec2 lastPlayerChunk; //Lưu vị trí chunk trước đó của player
    std::string lastQuadrant;  // Lưu góc phần tư trước đó: "UR", "UL", "LR", "LL"

    std::queue<ax::Vec2> chunkLoadQueue;  // Hàng đợi chunk cần load, (tránh load toàn bộ trong init - lag)
    bool isLoadingChunks = false;

    void loadChunk(const ax::Vec2& chunkPos);  // Load một chunk cụ thể
    void scheduleChunkLoading();   // Lên lịch load chunk
    void loadNextChunk(float dt);  // Load chunk tiếp theo trong queue
    void updateChunkVisibility(const ax::Vec2& playerPos, const ax::Vec2& currentChunk, const std::string& currentQuadrant); //SetVisible cho chunk

    ax::Vec2 getChunkCoordFromPosition(const ax::Vec2& pos);
    std::vector<ax::Vec2> getQuadrantChunks(const std::string& quadrant, const ax::Vec2& chunkPos);
    std::string getRandomTMXFile(const std::vector<std::string>& tmxFiles);

    void drawCollisionOutline(Chunk& chunk);  // Hàm vẽ viền để xem Collision

public:
    const std::vector<std::vector<Chunk>>& getGrid() const { return grid; }
    ax::Vec2 getChunkSize() const { return chunkSize; }  
    ax::Size getTileSize() const { return ax::Size(16, 16); } // Hardcode 16x16
    ax::Vec2 getGridSize() const { return gridSize; }
};



#endif  // __MAP_SYSTEM_H__
