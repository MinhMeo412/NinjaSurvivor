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
    if (cooldown->cooldownTimer == 0.0f)
    {
        // Bật hitbox trong 1 frame
        hitbox->size = hitbox->defaultSize;

        // Hiển thị hoạt ảnh chém
        SystemManager::getInstance()->getSystem<RenderSystem>()->updateSwordEntitySprite(weapon);

        cooldown->cooldownTimer = cooldown->cooldownDuration;
    }
}

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

