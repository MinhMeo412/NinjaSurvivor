#include "WeaponMovementSystem.h"

#include "SpawnSystem.h"
#include "CollisionSystem.h"
#include "CleanupSystem.h"
#include "WeaponSystem.h"
#include "SystemManager.h"
#include "Utils.h"

WeaponMovementSystem::WeaponMovementSystem(EntityManager& em,
                                           ComponentManager<IdentityComponent>& im,
                                           ComponentManager<TransformComponent>& tm,
                                           ComponentManager<VelocityComponent>& vm,
                                           ComponentManager<AnimationComponent>& am,
                                           ComponentManager<HitboxComponent>& hm,
                                           ComponentManager<SpeedComponent>& spm)
    : entityManager(em)
    , identityMgr(im)
    , transformMgr(tm)
    , velocityMgr(vm)
    , animationMgr(am)
    , hitboxMgr(hm)
    , speedMgr(spm)
{
    // Khai báo các kiểu di chuyển
    movementStrategies["enemy_projectile_energy_ball"]      = [this](Entity e, float dt) { moveEnemyProjectile(e, dt); };

    movementStrategies["weapon_melee_sword"]                = [this](Entity e, float dt) { moveSwordWeapon(e, dt); };
    movementStrategies["weapon_melee_shuriken"]             = [this](Entity e, float dt) { moveShurikenWeapon(e, dt); };
    movementStrategies["weapon_projectile_kunai"]           = [this](Entity e, float dt) { moveKunaiWeapon(e, dt); };
    movementStrategies["weapon_projectile_big_kunai"]       = [this](Entity e, float dt) { moveBigKunaiWeapon(e, dt); };
    movementStrategies["weapon_projectile_explosion_kunai"] = [this](Entity e, float dt) { moveExplosionKunaiWeapon(e, dt); };
    movementStrategies["weapon_projectile_ninjutsu_spell"] = [this](Entity e, float dt) { moveExplosionKunaiWeapon(e, dt); };
}

void WeaponMovementSystem::init() {}

// update toàn bộ hệ thống di chuyển, gọi mỗi frame
void WeaponMovementSystem::update(float dt)
{
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
    ax::Vec2 currentPos =
        ax::Vec2(transformMgr.getComponent(entity)->x, transformMgr.getComponent(entity)->y);  // Lấy vị trí hiện tại
    ax::Vec2 moveStep = ax::Vec2(velocityMgr.getComponent(entity)->vx, velocityMgr.getComponent(entity)->vy) *
                        dt;  // Một bước di chuyển (Vec2) trong 1 frame

    // Thêm giới hạn tốc độ để bỏ tính toán sub stepping (giới hạn trong khoảng 500speed)
    /*
    float stepLength    = moveStep.length();                          // Tính số pixel di chuyển trong 1 frame
    ax::Size tileSize =
        SystemManager::getInstance()->getSystem<MapSystem>()->getTileSize();  // Lấy kích thước 1 ô trong tileMap(pixel)

    if (stepLength > tileSize.width * 0.5f)
    {
        // Số bước nhỏ cần thực hiện trong 1 frame
        // ceil làm phép chia luôn trả về kết quả >= 1 trừ khi tử số = 0 (làm tròn để số bước di chuyển luôn ít nhất là
    1)
        // Chia cho nửa tileSize để tránh một bước dài hơn 1 tileSize (tunneling)
        int steps = std::min(3, static_cast<int>(std::ceil(stepLength / (tileSize.width * 0.5f))));  // Giới hạn tối đa
    3 bước (giảm tính toán)

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

// Enemy projectile
void WeaponMovementSystem::moveEnemyProjectile(Entity entity, float dt)
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

// Sword
void WeaponMovementSystem::moveSwordWeapon(Entity entity, float dt)
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
    auto playerVel       = velocityMgr.getComponent(player);
    if (!playerTransform)
        return;

    swordCountInFrame++;
    float offsetX = 8.0 + hitbox->defaultSize.width / 2;  // Khoảng cách từ nhân vật đến kiếm

    if (swordCountInFrame == 1)  // Nhát chém thứ 1
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

    if (swordCountInFrame == 2)  // Nhát chém thứ 2
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

// Shuriken
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

    // Các thông số cho chuyển động tròn
    float radius             = hitbox->defaultSize.x * 2;  // Bán kính quỹ đạo
    const float angularSpeed = 0.67f * 3.14159f;           // Tốc độ góc (1 vòng/giây)

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

// Kunai
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

// Hàm hỗ trợ cho weapon Shuriken
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

// Big Kunai
void WeaponMovementSystem::moveBigKunaiWeapon(Entity entity, float dt)
{
    auto transform = transformMgr.getComponent(entity);
    auto vel       = velocityMgr.getComponent(entity);
    auto speed     = speedMgr.getComponent(entity);
    auto hitbox    = hitboxMgr.getComponent(entity);
    if (!transform || !vel || !speed)
    {
        return;
    }

    // Kích thước màn hình cố định
    const float screenWidth  = 1280.0f;
    const float screenHeight = 720.0f;
    const float kunaiSize    = hitbox->defaultSize.width;

    // Lấy vị trí player
    auto playerPos = SystemManager::getInstance()->getSystem<SpawnSystem>()->getPlayerPosition();

    // Tính toán rìa màn hình dựa trên playerPos
    float leftEdge   = playerPos.x - screenWidth / 2.0f;
    float rightEdge  = playerPos.x + screenWidth / 2.0f;
    float topEdge    = playerPos.y + screenHeight / 2.0f;
    float bottomEdge = playerPos.y - screenHeight / 2.0f;

    // Cập nhật vị trí kunai
    transform->x += vel->vx * speed->speed * 2 * dt;
    transform->y += vel->vy * speed->speed * 2 * dt;

    // Kiểm tra va chạm với rìa màn hình
    // Rìa trái || Rìa phải
    if (transform->x < (leftEdge + kunaiSize) && vel->vx < 0)
    {
        vel->vx = -vel->vx;  // Đảo ngược vận tốc x
    }
    else if (transform->x > (rightEdge - kunaiSize) && vel->vx > 0)
    {
        vel->vx = -vel->vx;  // Đảo ngược vận tốc x
    }
    // Rìa trên || Rìa dưới
    if (transform->y > (topEdge - kunaiSize) && vel->vy > 0)
    {
        vel->vy = -vel->vy;  // Đảo ngược vận tốc y
    }
    else if (transform->y < (bottomEdge + kunaiSize) && vel->vy < 0)
    {
        vel->vy = -vel->vy;  // Đảo ngược vận tốc y
    }
}

// Spinner
void WeaponMovementSystem::moveSpinnerWeapon(Entity entity, float dt)
{
    auto spawnSystem     = SystemManager::getInstance()->getSystem<SpawnSystem>();
    auto collisionSystem = SystemManager::getInstance()->getSystem<CollisionSystem>();
    if (!spawnSystem || !collisionSystem)
        return;

    auto enemiesInView = collisionSystem->getEnemyEntitiesInView(spawnSystem->getPlayerPosition());

    auto transform = transformMgr.getComponent(entity);
    if (!enemiesInView.empty())
    {
        int index     = Utils::getRandomInt(0, enemiesInView.size() - 1);
        auto enemyPos = transformMgr.getComponent(enemiesInView[index]);
        transform->x  = enemyPos->x + Utils::getRandomFloat(-10, 10);
        transform->y  = enemyPos->y + Utils::getRandomFloat(-10, 10);
    }
    else
    {
        transform->x = spawnSystem->getPlayerPosition().x + Utils::getRandomFloat(-150, 150);
        transform->y = spawnSystem->getPlayerPosition().y + Utils::getRandomFloat(-300, 300);
    }
}

// Explosion kunai
void WeaponMovementSystem::moveExplosionKunaiWeapon(Entity entity, float dt)
{
    auto transform = transformMgr.getComponent(entity);
    auto vel       = velocityMgr.getComponent(entity);
    auto speed     = speedMgr.getComponent(entity);

    transform->x += vel->vx * speed->speed * dt;
    transform->y += vel->vy * speed->speed * dt;
}

// Ninjutsu Spell
void WeaponMovementSystem::moveNinjutsuSpellWeapon(Entity entity, float dt)
{
    auto transform = transformMgr.getComponent(entity);
    auto vel       = velocityMgr.getComponent(entity);
    auto speed     = speedMgr.getComponent(entity);

    transform->x += vel->vx * speed->speed * dt;
    transform->y += vel->vy * speed->speed * dt;
}

// Lightning Scroll
void WeaponMovementSystem::moveLightningScrollWeapon(Entity entity, float dt)
{
    auto spawnSystem     = SystemManager::getInstance()->getSystem<SpawnSystem>();
    auto collisionSystem = SystemManager::getInstance()->getSystem<CollisionSystem>();
    if (!spawnSystem || !collisionSystem)
        return;

    auto enemiesInView = collisionSystem->getEnemyEntitiesInView(spawnSystem->getPlayerPosition());

    auto transform = transformMgr.getComponent(entity);
    if (!enemiesInView.empty())
    {
        int index     = Utils::getRandomInt(0, enemiesInView.size() - 1);
        auto enemyPos = transformMgr.getComponent(enemiesInView[index]);
        transform->x  = enemyPos->x;  //+ Utils::getRandomFloat(-10, 10);
        transform->y  = enemyPos->y;  //+ Utils::getRandomFloat(-10, 10);
    }
    else
    {
        transform->x = spawnSystem->getPlayerPosition().x + Utils::getRandomFloat(-150, 150);
        transform->y = spawnSystem->getPlayerPosition().y + Utils::getRandomFloat(-300, 300);
    }
}
