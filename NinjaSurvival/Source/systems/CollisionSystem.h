#ifndef __COLLISION_SYSTEM_H__
#define __COLLISION_SYSTEM_H__

#include "System.h"
#include "components/ComponentManager.h"
#include "entities/EntityManager.h"
#include "axmol.h"

class CollisionSystem : public System
{
public:
    CollisionSystem(EntityManager& em,
                 ComponentManager<IdentityComponent>& im,
                 ComponentManager<TransformComponent>& tm,
                 ComponentManager<HitboxComponent>& hm); 

    void init() override;
    void update(float dt) override;

    // Xử lý vị trí mới của entity khi di chuyển, trả về vị trí đã điều chỉnh nếu có va chạm
    ax::Vec2 resolvePosition(Entity entity, const ax::Vec2& newPos);
    // Kiểm tra va chạm giữa entity và collision tile tại vị trí cụ thể
    bool isCollidingWithTileMap(Entity entity, const ax::Vec2& position);

private:
    EntityManager& entityManager;
    ComponentManager<IdentityComponent>& identityMgr;
    ComponentManager<TransformComponent>& transformMgr;
    ComponentManager<HitboxComponent>& hitboxMgr;

    // Cấu trúc SpatialGrid (Spatial Partitioning) để tối ưu hóa kiểm tra va chạm
    struct SpatialGrid
    {
        std::vector<std::vector<std::vector<Entity>>> cells;// Lưới chứa các entity theo ô
        ax::Vec2 cellSize = ax::Vec2(64, 64);               // Kích thước mỗi ô trong lưới
        ax::Vec2 gridSize;                                  // Kích thước lưới

        void init(const ax::Vec2& worldSize);               // Khởi tạo lưới với kích thước tổng của map
        void clear();                                       // Xóa toàn bộ entity trong lưới
        void insert(Entity entity, const ax::Vec2& pos);    // Thêm entity vào ô tương ứng
        std::vector<Entity> getNearbyEntities(const ax::Vec2& pos); // Lấy các entity gần vị trí
    };

    SpatialGrid spatialGrid; // Khởi tạo lưới

    std::function<void(Entity, Entity)> onCollision; // Callback khi va chạm xảy ra

    // Kiểm tra va chạm giữa 2 entity
    bool checkCollision(Entity e1, Entity e2);
    // Chuyển đổi tọa độ pixel sang tọa độ tile trong tile map
    ax::Vec2 convertToTileCoord(const ax::Vec2& position, ax::TMXTiledMap* tiledMap);
    // Lấy tile map của chunk tại vị trí cụ thể
    ax::TMXTiledMap* getChunkTileMap(const ax::Vec2& position);
};




#endif  // __COLLISION_SYSTEM_H__
