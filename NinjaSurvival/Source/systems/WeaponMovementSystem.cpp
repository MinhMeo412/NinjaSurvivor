#include "WeaponMovementSystem.h"

#include "SpawnSystem.h"
#include "CollisionSystem.h"
#include "CleanupSystem.h"
#include "WeaponSystem.h"
#include "SystemManager.h"

WeaponMovementSystem::WeaponMovementSystem(EntityManager& em,
                               ComponentManager<IdentityComponent>& im,
                               ComponentManager<TransformComponent>& tm,
                               ComponentManager<VelocityComponent>& vm,
                               ComponentManager<AnimationComponent>& am,
                               ComponentManager<HitboxComponent>& hm,
                               ComponentManager<SpeedComponent>& spm)
    : entityManager(em), identityMgr(im), transformMgr(tm), velocityMgr(vm), animationMgr(am), hitboxMgr(hm), speedMgr(spm)
{
    // Khai báo các kiểu di chuyển
    movementStrategies["weapon_melee_sword"]  = [this](Entity e, float dt) { moveSwordWeapon(e, dt); };
    movementStrategies["weapon_melee_shuriken"]  = [this](Entity e, float dt) { moveShurikenWeapon(e, dt); };
    movementStrategies["weapon_projectile_kunai"]  = [this](Entity e, float dt) { moveKunaiWeapon(e, dt); };


}

void WeaponMovementSystem::init() {}

// update toàn bộ hệ thống di chuyển, gọi mỗi frame
void WeaponMovementSystem::update(float dt)
{
    auto start = std::chrono::high_resolution_clock::now();

    auto collisionSystem = SystemManager::getInstance()->getSystem<CollisionSystem>();
    if (!collisionSystem)
        return;

    // Reset biến đếm kiếm mỗi frame
    swordCountInFrame = 0;

    // Xử lý move của weapon
    auto weaponPool = SystemManager::getInstance()->getSystem<WeaponSystem>()->getWeaponEntities();
    for (auto& weapon : weaponPool)
    {
        if (identityMgr.getComponent(weapon)->name != "kunai")
        {
            updateEntityMovement(weapon, dt);
        }
    }
    auto tempWeaponPool = SystemManager::getInstance()->getSystem<WeaponSystem>()->getTempWeaponEntities();
    for (auto& weapon : tempWeaponPool)
    {
        updateEntityMovement(weapon, dt);
    }


    auto end      = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    //AXLOG("Thời gian thực thi WeaponMovementSystem: %ld ms", duration);
}

// Hàm cập nhật di chuyển cho entity
void WeaponMovementSystem::updateEntityMovement(Entity entity, float dt)
{
    auto transform = transformMgr.getComponent(entity);
    if (!transform)
        return;

    // Xác định key để tìm kiểu di chuyển
    std::string key;
    if (auto identity = identityMgr.getComponent(entity))
    {
        key = identity->type + "_" + identity->name;

        auto it = movementStrategies.find(key);
        if (it != movementStrategies.end())
        {
            // Gọi hàm di chuyển tương ứng
            it->second(entity, dt);
        }
    }
}

// Xử lý sub-stepping và đặt vị trí mới
ax::Vec2 WeaponMovementSystem::subSteppingHandle(Entity entity, float dt)
{
    // Sub-stepping
    ax::Vec2 currentPos = ax::Vec2(transformMgr.getComponent(entity)->x, transformMgr.getComponent(entity)->y);  // Lấy vị trí hiện tại
    ax::Vec2 moveStep   = ax::Vec2(velocityMgr.getComponent(entity)->vx, velocityMgr.getComponent(entity)->vy) * dt;  // Một bước di chuyển (Vec2) trong 1 frame

    // Thêm giới hạn tốc độ để bỏ tính toán sub stepping (giới hạn trong khoảng 500speed)
    /*
    float stepLength    = moveStep.length();                          // Tính số pixel di chuyển trong 1 frame
    ax::Size tileSize =
        SystemManager::getInstance()->getSystem<MapSystem>()->getTileSize();  // Lấy kích thước 1 ô trong tileMap(pixel)
    
    if (stepLength > tileSize.width * 0.5f)
    {
        // Số bước nhỏ cần thực hiện trong 1 frame
        // ceil làm phép chia luôn trả về kết quả >= 1 trừ khi tử số = 0 (làm tròn để số bước di chuyển luôn ít nhất là 1)
        // Chia cho nửa tileSize để tránh một bước dài hơn 1 tileSize (tunneling)
        int steps = std::min(3, static_cast<int>(std::ceil(stepLength / (tileSize.width * 0.5f))));  // Giới hạn tối đa 3 bước (giảm tính toán)

        // Một bước nhỏ trong Tổng số bước cần thực hiện/frame
        // Đảm bảo luôn chia được kể cả khi không di chuyển (steps = 0)
        ax::Vec2 step = moveStep / (steps > 0 ? steps : 1);

        // Khai báo biến mới để tính toán vị trí nhân vật di chuyển tiếp theo trong frame
        ax::Vec2 adjustedPos = currentPos;
        auto collisionSystem = SystemManager::getInstance()->getSystem<CollisionSystem>();
        for (int i = 0; i < steps; ++i)
        {
            ax::Vec2 newPos = adjustedPos + step;
            // Kiểm tra và điều chỉnh vị trí với CollisionSystem để tránh va chạm
            adjustedPos =
                collisionSystem->resolvePosition(entity, newPos);  // Nhận giá trị vị trí mới với mỗi step trong steps
        }
        // Cập nhật vị trí entity sau khi tính toán
        transform->x = adjustedPos.x;
        transform->y = adjustedPos.y;

        return adjustedPos;
    }
    else
    {*/
        // Di chuyển trực tiếp nếu vận tốc nhỏ
        ax::Vec2 newPos = currentPos + moveStep;
        newPos          = SystemManager::getInstance()->getSystem<CollisionSystem>()->resolvePosition(entity, newPos);
        transformMgr.getComponent(entity)->x = newPos.x;
        transformMgr.getComponent(entity)->y = newPos.y;
        return newPos;
    //}
}

//Sword
void WeaponMovementSystem::moveSwordWeapon(Entity entity, float dt)
{
    auto transform = transformMgr.getComponent(entity);
    auto vel       = velocityMgr.getComponent(entity);
    auto hitbox    = hitboxMgr.getComponent(entity);
    if (!transform || !vel || !hitbox)
        return;


    //AXLOG("Size: %f,%f", hitbox->defaultSize.width, hitbox->defaultSize.height);

    auto spawnSystem = SystemManager::getInstance()->getSystem<SpawnSystem>();
    if (!spawnSystem)
        return;

    // Lấy vị trí player
    Entity player        = spawnSystem->getPlayerEntity();
    auto playerTransform = transformMgr.getComponent(player);
    auto playerVel = velocityMgr.getComponent(player);
    if (!playerTransform)
        return;

    swordCountInFrame++;
    float offsetX = 8.0 + hitbox->defaultSize.width/2;  // Khoảng cách từ nhân vật đến kiếm
        //AXLOG("OffsetX: %f", offsetX);

    if (swordCountInFrame == 1) //Nhát chém thứ 1
    {
        if (playerVel->vx > 0)  // Sang phải
        {
            transform->x = playerTransform->x + offsetX;
            vel->vx      = 1.0f;  // Lưu hướng sang phải
        }
        else if (playerVel->vx < 0)  // Sang trái
        {
            transform->x = playerTransform->x - offsetX;
            vel->vx      = -1.0f;  // Lưu hướng sang trái
        }
        else
        {
            if (vel->vx == 0.0f)
            {
                transform->x = playerTransform->x + offsetX;  // Mặc định bên phải
            }
            else
            {
                transform->x = (vel->vx > 0) ? (playerTransform->x + offsetX) : (playerTransform->x - offsetX);
            }
        }
    }
    
    if (swordCountInFrame == 2) //Nhát chém thứ 2
    {
        if (playerVel->vx > 0)  // Sang phải (chém sau)
        {
            transform->x = playerTransform->x - offsetX;
            vel->vx      = -1.0f;  // Lưu hướng sang trái
        }
        else if (playerVel->vx < 0)  // Sang trái (chém sau)
        {
            transform->x = playerTransform->x + offsetX;
            vel->vx      = 1.0f;  // Lưu hướng sang phải
        }
        else
        {
            if (vel->vx == 0.0f)
            {
                transform->x = playerTransform->x - offsetX;  // Mặc định bên trái
            }
            else
            {
                transform->x = (vel->vx > 0) ? (playerTransform->x + offsetX) : (playerTransform->x - offsetX);
            }
        }
    }

    transform->y = playerTransform->y;
}

void WeaponMovementSystem::moveShurikenWeapon(Entity entity, float dt)
{
    auto transform = transformMgr.getComponent(entity);
    auto vel       = velocityMgr.getComponent(entity);
    auto hitbox    = hitboxMgr.getComponent(entity);
    if (!transform || !vel || !hitbox)
        return;

    auto spawnSystem = SystemManager::getInstance()->getSystem<SpawnSystem>();
    if (!spawnSystem)
        return;
    // Lấy vị trí player
    Entity player        = spawnSystem->getPlayerEntity();
    auto playerTransform = transformMgr.getComponent(player);
    if (!playerTransform)
        return;

    // Đếm số shuriken trong weaponPool có type là shuriken
    auto weaponPool   = SystemManager::getInstance()->getSystem<WeaponSystem>()->getWeaponEntities();
    int shurikenCount = 0;
    for (size_t i = 0; i < weaponPool.size(); ++i)
    {
        auto identity = identityMgr.getComponent(weaponPool[i]);
        if (identity && identity->name == "shuriken")
        {
            shurikenCount++;
        }
    }

    // Các thông số cho chuyển động tròn 
    float radius             = hitbox->defaultSize.x * 2;      // Bán kính quỹ đạo
    const float angularSpeed = 0.67f * 3.14159f; // Tốc độ góc (1 vòng/giây)

    // Cập nhật góc
    vel->vx += angularSpeed * dt;
    if (vel->vx > 2.0f * 3.14159f)
    {
        vel->vx -= 2.0f * 3.14159f;  // Giữ góc trong khoảng [0, 2π]
    }

    // Tính toán vị trí mới
    float newX = playerTransform->x + radius * cos(vel->vx);
    float newY = playerTransform->y + radius * sin(vel->vx);

    // Cập nhật vị trí
    transform->x = newX;
    transform->y = newY;
}

void WeaponMovementSystem::moveKunaiWeapon(Entity entity, float dt)
{
    auto transform = transformMgr.getComponent(entity);
    auto vel       = velocityMgr.getComponent(entity);
    auto speed     = speedMgr.getComponent(entity);
    if (!transform || !vel || !speed)
    {
        return;
    }

    transform->x += vel->vx * speed->speed * dt;
    transform->y += vel->vy * speed->speed * dt;
}


void WeaponMovementSystem::recalculateShurikenAngles(const std::vector<Entity>& shurikenList)
{
    // Số lượng shuriken
    int shurikenCount = static_cast<int>(shurikenList.size());

    // Phân bổ góc đều cho mỗi shuriken
    for (size_t i = 0; i < shurikenList.size(); ++i)
    {
        auto vel = velocityMgr.getComponent(shurikenList[i]);
        if (vel)
        {
            vel->vx = static_cast<float>(i) * (2.0f * 3.14159f / shurikenCount);
        }
    }
}
