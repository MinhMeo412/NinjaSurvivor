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

    // Callback để cập nhật visibility khi player đổi chunk
    void onPlayerPositionChanged(const ax::Vec2& playerPos);

private:        
    ax::Scene* parentScene   = nullptr;

    struct Chunk
    {
        ax::TMXTiledMap* tiledMap = nullptr;
        ax::Vec2 gridPos;
        bool isVisible = false;
    };

    std::vector<std::vector<Chunk>> grid;
    ax::Vec2 chunkSize;
    ax::Vec2 gridSize;
    ax::Vec2 lastPlayerChunk;
    std::string lastQuadrant;  // Lưu góc phần tư trước đó: "UR", "UL", "LR", "LL"

    void loadMapChunks();
    void updateChunkVisibility(const ax::Vec2& playerPos, const ax::Vec2& currentChunk, const std::string& currentQuadrant);
    ax::Vec2 getChunkCoordFromPosition(const ax::Vec2& pos);
    std::vector<ax::Vec2> getQuadrantChunks(const std::string& quadrant, const ax::Vec2& chunkPos);
    std::string getRandomTMXFile(const std::vector<std::string>& tmxFiles);

public:
    const std::vector<std::vector<Chunk>>& getGrid() const { return grid; }
    ax::Vec2 getChunkSize() const { return chunkSize; }  
    ax::Size getTileSize() const { return ax::Size(16, 16); } // Hardcode 16x16
    ax::Vec2 getGridSize() const { return gridSize; }
};



#endif  // __MAP_SYSTEM_H__
