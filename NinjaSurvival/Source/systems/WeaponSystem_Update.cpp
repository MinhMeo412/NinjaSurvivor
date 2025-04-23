#include "Utils.h"
#include "WeaponSystem.h"
#include "EntityFactory.h"
#include "SpawnSystem.h"
#include "MovementSystem.h"
#include "CollisionSystem.h"
#include "TimeSystem.h"
#include "LevelSystem.h"
#include "RenderSystem.h"
#include "SystemManager.h"
#include "GameData.h"
#include "AudioManager.h"

//Enemy projectile
void WeaponSystem::updateEnemyProjectile(Entity weapon, float dt)
{
    auto duration = durationMgr.getComponent(weapon);

    duration->duration -= dt;
}

//Sword
void WeaponSystem::updateSword(Entity weapon,float dt)
{
    auto cooldown = cooldownMgr.getComponent(weapon);
    auto hitbox   = hitboxMgr.getComponent(weapon);
    if (!cooldown || !hitbox)
        return;

    // Kiểm tra để tắt hitbox
    if (cooldown->cooldownTimer > 0.0f && hitbox->size.width > 0)
    {
        hitbox->size = ax::Size(0, 0);
    }

    // Khi cooldownTimer chính xác bằng 0, thực hiện chém
    else if (cooldown->cooldownTimer == 0.0f)
    {
        // Bật hitbox trong 1 frame
        hitbox->size = hitbox->defaultSize;

        // Hiển thị hoạt ảnh chém
        SystemManager::getInstance()->getSystem<RenderSystem>()->updateSwordEntitySprite(weapon);
        AudioManager::getInstance()->playSound("sword", false, 1.0f, "effect");

        cooldown->cooldownTimer = cooldown->cooldownDuration;
    }
}

//Shuriken
void WeaponSystem::updateShuriken(Entity weapon, float dt)
{
    // Chia update theo từng loại weapon
    auto cooldown = cooldownMgr.getComponent(weapon);
    auto duration = durationMgr.getComponent(weapon);
    auto hitbox   = hitboxMgr.getComponent(weapon);
    if (!cooldown || !hitbox)
        return;

    // Check level shuriken
    auto weaponInventory = weaponInventoryMgr.getComponent(playerEntity);
    auto it              = std::find_if(weaponInventory->weapons.begin(), weaponInventory->weapons.end(),
                                        [](auto& p) { return p.first == "shuriken"; });

    if (it->second == 5)  // Active vĩnh viễn
    {
        cooldown->cooldownDuration = 0;
        cooldown->cooldownTimer    = 1.0;
        // Bật hitbox
        hitbox->size = hitbox->defaultSize;
    }
    else
    {
        // Kiểm tra để tắt hitbox
        if (cooldown->cooldownTimer <= cooldown->cooldownDuration && hitbox->size.width > 0)
        {
            hitbox->size = ax::Size(0, 0);
        }

        // Khi cooldownTimer chính xác bằng 0, thực hiện xoay
        if (cooldown->cooldownTimer == 0.0f)
        {
            // Bật hitbox tgian active
            hitbox->size = hitbox->defaultSize;

            cooldown->cooldownTimer = cooldown->cooldownDuration + duration->duration;  // Thời gian active
        }
    }
}

//Kunai
void WeaponSystem::updateKunai(Entity weapon, float dt)
{
    auto cooldown = cooldownMgr.getComponent(kunaiEntity);
    auto vel = velocityMgr.getComponent(kunaiEntity);
    auto playerVel = velocityMgr.getComponent(playerEntity);

    if (playerVel->vx != 0 || playerVel->vy != 0)
    {
        ax::Vec2 newVel = ax::Vec2(playerVel->vx, playerVel->vy).getNormalized();
        vel->vx             = newVel.x;
        vel->vy             = newVel.y;
    }

    // Spawn kunai mỗi cooldownDuration của kunai mẫu
    if (cooldown->cooldownTimer == 0)
    {
        tempWeaponPool.push_back(createTempKunai("kunai"));
        cooldown->cooldownTimer = cooldown->cooldownDuration;
    }
}

//Big Kunai
void WeaponSystem::updateBigKunai(Entity weapon, float dt)
{
    auto cooldown = cooldownMgr.getComponent(weapon);
    auto duration = durationMgr.getComponent(weapon);
    auto hitbox   = hitboxMgr.getComponent(weapon);
    auto vel      = velocityMgr.getComponent(weapon);

    if (cooldown->cooldownTimer == 0)
    {
        auto transform = transformMgr.getComponent(weapon);
        transform->x   = transformMgr.getComponent(playerEntity)->x;
        transform->y   = transformMgr.getComponent(playerEntity)->y;

        ax::Vec2 newVel = ax::Vec2(Utils::getRandomFloat(-1.0, 1.0), Utils::getRandomFloat(-1.0, 1.0)).getNormalized();
        vel->vx         = newVel.x;
        vel->vy         = newVel.y;

        cooldown->cooldownTimer = duration->duration + cooldown->cooldownDuration;

        hitbox->size = hitbox->defaultSize;
    }
    else if (cooldown->cooldownTimer <= cooldown->cooldownDuration && hitbox->size.width > 0)
    {
        hitbox->size = ax::Size(0, 0);
    }
}

// Spinner
void WeaponSystem::updateSpinner(Entity weapon, float dt)
{
    auto cooldown = cooldownMgr.getComponent(weapon);
    auto duration = durationMgr.getComponent(weapon);
    auto hitbox   = hitboxMgr.getComponent(weapon);
    auto transform = transformMgr.getComponent(weapon);

    if (cooldown->cooldownTimer == 0)
    {
        SystemManager::getInstance()->getSystem<MovementSystem>()->getWeaponMoveSystem()->moveSpinnerWeapon(weapon, dt);
        cooldown->cooldownTimer = duration->duration + cooldown->cooldownDuration;
        hitbox->size = hitbox->defaultSize;
        // Xử lý hình ảnh
        SystemManager::getInstance()->getSystem<RenderSystem>()->updateSpinnerEntitySprite(weapon);
    }
    else if (cooldown->cooldownTimer <= cooldown->cooldownDuration && hitbox->size.width > 0)
    {
        hitbox->size = ax::Size(0, 0);
        // Xử lý hình ảnh
        SystemManager::getInstance()->getSystem<RenderSystem>()->updateSpinnerEntitySprite(weapon);
    }
}

// Explosion Kunai
void WeaponSystem::updateExplosionKunai(Entity weapon, float dt)
{
    auto cooldown  = cooldownMgr.getComponent(weapon);
    auto duration  = durationMgr.getComponent(weapon);
    auto hitbox    = hitboxMgr.getComponent(weapon);
    auto transform = transformMgr.getComponent(weapon);
    auto velocity  = velocityMgr.getComponent(weapon);

    if (cooldown->cooldownTimer == 0)
    {
        // Đặt vị trí về player
        auto playerTransform = transformMgr.getComponent(playerEntity);
        transform->x         = playerTransform->x;
        transform->y         = playerTransform->y;

        // Tính vel mới
        auto collisionSystem = SystemManager::getInstance()->getSystem<CollisionSystem>();
        if (!collisionSystem)
            return;
        auto enemiesInView = collisionSystem->getEnemyEntitiesInView(ax::Vec2(transform->x, transform->y));
        if (!enemiesInView.empty())
        {
            int index        = Utils::getRandomInt(0, enemiesInView.size() - 1);
            auto enemyPos    = transformMgr.getComponent(enemiesInView[index]);
            float directionX = enemyPos->x - transform->x;
            float directionY = enemyPos->y - transform->y;
            ax::Vec2 newVel  = ax::Vec2(directionX, directionY).getNormalized();

            velocity->vx = newVel.x;
            velocity->vy = newVel.y;
        }
        else
        {
            float directionX = Utils::getRandomFloat(-1.0, 1.0);
            float directionY = Utils::getRandomFloat(-1.0, 1.0);
            ax::Vec2 newVel  = ax::Vec2(directionX, directionY).getNormalized();
            velocity->vx     = newVel.x;
            velocity->vy     = newVel.y;
        }

        cooldown->cooldownTimer = duration->duration + cooldown->cooldownDuration;
        hitbox->size            = hitbox->defaultSize;
    }
    else if (cooldown->cooldownTimer <= cooldown->cooldownDuration && hitbox->size.width > 0)
    {
        hitbox->size = ax::Size(0, 0);
    }
}

// Ninjutsu Spell
void WeaponSystem::updateNinjutsuSpell(Entity weapon, float dt)
{
    auto cooldown  = cooldownMgr.getComponent(weapon);
    auto duration  = durationMgr.getComponent(weapon);
    auto hitbox    = hitboxMgr.getComponent(weapon);
    auto transform = transformMgr.getComponent(weapon);
    auto velocity  = velocityMgr.getComponent(weapon);

    if (cooldown->cooldownTimer == 0)
    {
        // Đặt vị trí về player
        auto playerTransform = transformMgr.getComponent(playerEntity);
        transform->x         = playerTransform->x;
        transform->y         = playerTransform->y;

        // Tính vel mới
        auto collisionSystem = SystemManager::getInstance()->getSystem<CollisionSystem>();
        if (!collisionSystem)
            return;
        auto enemiesInView = collisionSystem->getEnemyEntitiesInView(ax::Vec2(transform->x, transform->y));
        if (!enemiesInView.empty())
        {
            // Tìm enemy gần nhất
            Entity closestEnemy  = enemiesInView[0];
            auto closestEnemyPos = transformMgr.getComponent(closestEnemy);
            float minDistance =
                ax::Vec2(transform->x, transform->y).getDistance(ax::Vec2(closestEnemyPos->x, closestEnemyPos->y));

            for (size_t i = 1; i < enemiesInView.size(); ++i)
            {
                auto enemyPos  = transformMgr.getComponent(enemiesInView[i]);
                float distance = ax::Vec2(transform->x, transform->y).getDistance(ax::Vec2(enemyPos->x, enemyPos->y));
                if (distance < 100.0f)
                {
                    // Nếu khoảng cách < 100, chọn enemy này và dừng duyệt
                    closestEnemy    = enemiesInView[i];
                    closestEnemyPos = enemyPos;
                    minDistance     = distance;
                    break;
                }
                if (distance < minDistance)
                {
                    // Cập nhật enemy gần nhất nếu không thỏa điều kiện < 100
                    minDistance     = distance;
                    closestEnemy    = enemiesInView[i];
                    closestEnemyPos = enemyPos;
                }
            }

            // Tính hướng mới dựa trên enemy gần nhất
            float directionX = closestEnemyPos->x - transform->x;
            float directionY = closestEnemyPos->y - transform->y;
            ax::Vec2 newVel  = ax::Vec2(directionX, directionY).getNormalized();

            velocity->vx = newVel.x;
            velocity->vy = newVel.y;
        }
        else
        {
            velocity->vx = 0.0;
            velocity->vy = 1.0;
        }

        cooldown->cooldownTimer = duration->duration + cooldown->cooldownDuration;
        hitbox->size            = hitbox->defaultSize;
    }
    else if (cooldown->cooldownTimer <= cooldown->cooldownDuration && hitbox->size.width > 0)
    {
        hitbox->size = ax::Size(0, 0);
    }
}

// Lightning Scroll
void WeaponSystem::updateLightningScroll(Entity weapon, float dt)
{
    auto cooldown = cooldownMgr.getComponent(weapon);
    auto hitbox   = hitboxMgr.getComponent(weapon);
    if (!cooldown || !hitbox)
        return;

    // Kiểm tra để tắt hitbox
    if (cooldown->cooldownTimer > 0.0f && hitbox->size.width > 0)
    {
        hitbox->size = ax::Size(0, 0);
    }

    // Khi cooldownTimer chính xác bằng 0, thực hiện
    else if (cooldown->cooldownTimer == 0.0f)
    {
        SystemManager::getInstance()->getSystem<MovementSystem>()->getWeaponMoveSystem()->moveLightningScrollWeapon(weapon, dt);

        // Hiển thị animation
        SystemManager::getInstance()->getSystem<RenderSystem>()->updateLightningScrollEntitySprite(weapon);

        AudioManager::getInstance()->playSound("lightning_scroll", false, 1.0f, "effect");

        // Bật hitbox trong 1 frame
        hitbox->size = hitbox->defaultSize;


        cooldown->cooldownTimer = cooldown->cooldownDuration;
    }
}
