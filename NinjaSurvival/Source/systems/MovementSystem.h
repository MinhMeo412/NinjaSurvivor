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
                   ComponentManager<HitboxComponent>& hm);

    void update(float dt) override;
    void init() override;

private:
    EntityManager& entityManager;
    ComponentManager<IdentityComponent>& identityMgr;
    ComponentManager<TransformComponent>& transformMgr;
    ComponentManager<VelocityComponent>& velocityMgr;
    ComponentManager<AnimationComponent>& animationMgr;
    ComponentManager<HitboxComponent>& hitboxMgr;

    // Định nghĩa một kiểu hàm MoveFunc nhận Entity và float, trả về void
    // std::function khai báo kiểu hàm với tên bất kỳ có 2 tham số và kiểu trả về là void
    using MoveFunc = std::function<void(Entity, float)>;
    // Một map lưu cặp key(kiểu string entity type) và value(kiểu MoveFunc)
    std::unordered_map<std::string, MoveFunc> movementStrategies;

    void updateEntityMovement(Entity entity, float dt);  // Hàm chung bao gồm xử lý sub-stepping
    // Các hàm di chuyển trả về velocity cho entity
    void movePlayer(Entity entity, float dt);      // Di chuyển player
    void moveEnemySlime(Entity entity, float dt);  // Di chuyển cho Slime
    void moveItem(Entity entity, float dt); //Khi item được "nhặt"
    // Thêm cjo các entity khác: moveBossX, moveProjectileY...

    // Unordered_map lưu thời gian giãn cách để cập nhật hướng di chuyển cho từng Slime
    // Có thể xem xét bỏ nếu không cần thiết (nếu bỏ thì phải kiểm tra quá nhiều entity mỗi frame, nếu giữ thì entity nào cũng cần có)
    std::unordered_map<Entity, float> slimeDirectionTimers;



};

#endif  // __MOVEMENT_SYSTEM_H__
