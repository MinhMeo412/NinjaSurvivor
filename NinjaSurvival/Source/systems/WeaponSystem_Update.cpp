#include "Utils.h"
#include "WeaponSystem.h"
#include "EntityFactory.h"
#include "SpawnSystem.h"
#include "MovementSystem.h"
#include "TimeSystem.h"
#include "LevelSystem.h"
#include "RenderSystem.h"
#include "SystemManager.h"
#include "GameData.h"

//Enemy projectile
void WeaponSystem::updateEnemyProjectile(Entity weapon, float dt)
{
    auto duration = durationMgr.getComponent(weapon);

    duration->duration -= dt;
}

//Sword
void WeaponSystem::updateSword(Entity weapon,float dt)
{
    // Chia update theo từng loại weapon
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
