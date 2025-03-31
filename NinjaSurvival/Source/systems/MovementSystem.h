#ifndef __MOVEMENT_SYSTEM_H__
#define __MOVEMENT_SYSTEM_H__

#include "System.h"
#include "components/ComponentManager.h"
#include "entities/EntityManager.h"

class MovementSystem : public System
{
public:
    MovementSystem(EntityManager& em,
                   ComponentManager<IdentityComponent>& im,
                   ComponentManager<TransformComponent>& tm,
                   ComponentManager<VelocityComponent>& vm,
                   ComponentManager<AnimationComponent>& am,
                   ComponentManager<HitboxComponent>& hm,
                   ComponentManager<SpeedComponent>& spm);

    void update(float dt) override;
    void init() override;

private:
    EntityManager& entityManager;
    ComponentManager<IdentityComponent>& identityMgr;
    ComponentManager<TransformComponent>& transformMgr;
    ComponentManager<VelocityComponent>& velocityMgr;
    ComponentManager<AnimationComponent>& animationMgr;
    ComponentManager<HitboxComponent>& hitboxMgr;
    ComponentManager<SpeedComponent>& speedMgr;

    // Định nghĩa một kiểu hàm MoveFunc nhận Entity và float, trả về void
    // std::function khai báo kiểu hàm với tên bất kỳ có 2 tham số và kiểu trả về là void
    using MoveFunc = std::function<void(Entity, float)>;
    // Một map lưu cặp key(kiểu string entity type) và value(kiểu MoveFunc)
    std::unordered_map<std::string, MoveFunc> movementStrategies;

    void updateEntityMovement(Entity entity, float dt);  // Hàm chung bao gồm xử lý sub-stepping
    // Các hàm di chuyển trả về velocity cho entity
    void movePlayer(Entity entity, float dt);       // Di chuyển player
    void moveMeleeEnemy(Entity entity, float dt);   // Di chuyển cho melee enemy
    void moveRangedEnemy(Entity entity, float dt);  // Di chuyển cho ranged enemy

    void moveItem(Entity entity, float dt); //Khi item được "nhặt"
    // Thêm các entity khác: moveBossX, moveProjectileY...

    //Re position nếu entity ra ngoài view quá xa
    bool isOutOfView(Entity entity);

    // Quyết định ngẫu nhiên xem có cập nhật hướng không
    std::random_device rd;                                  // Seed cho random
    std::mt19937 gen{rd()};                                 // Generator ngẫu nhiên
    std::uniform_real_distribution<float> dis{0.0f, 1.0f};  // Phân phối từ 0 đến 1
    const float updateProbability = 1.0f / 10.0f;           // Tỷ lệ 10% mỗi frame


    std::unordered_map<Entity, float> timers;   // Timer riêng cho ranged enemy
    std::uniform_real_distribution<float> distance{150.0f, 360.0f}; // Khoảng cách cho ranged enemy với player
};

#endif  // __MOVEMENT_SYSTEM_H__
