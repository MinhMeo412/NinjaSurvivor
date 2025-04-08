#include "WeaponSystem.h"
#include "EntityFactory.h"
#include "SpawnSystem.h"
#include "RenderSystem.h"
#include "SystemManager.h"
#include "GameData.h"

void WeaponSystem::init()
{
    // Khởi tạo entity factory
    factory = std::make_unique<EntityFactory>(entityManager, identityMgr, transformMgr, spriteMgr, animationMgr,
                                              velocityMgr, hitboxMgr, healthMgr, attackMgr, cooldownMgr, speedMgr, weaponInventoryMgr);

    auto spawnSystem = SystemManager::getInstance()->getSystem<SpawnSystem>();
    if (spawnSystem)
    {
        playerEntity = spawnSystem->getPlayerEntity();
    }

    initializePlayerWeapon(playerEntity);
}

void WeaponSystem::update(float dt)
{
    for (auto& weapon : weaponPool)
    {
        auto cooldown  = cooldownMgr.getComponent(weapon);
        auto hitbox   = hitboxMgr.getComponent(weapon);
        if (!cooldown || !hitbox)
            continue;

        // Kiểm tra để tắt hitbox
        if (cooldown->cooldownTimer > 0.0f && hitbox->size.width > 0)
        {
            hitbox->size = ax::Size(0, 0);
        }

        // Khi cooldownTimer chính xác bằng 0, thực hiện chém
        if (cooldown->cooldownTimer == 0.0f)
        {
            // Bật hitbox trong 1 frame
            hitbox->size = ax::Size(32, 16);
            AXLOG("Sword %u slashes! Hitbox activated.", weapon);

            // Hiển thị hoạt ảnh chém
            SystemManager::getInstance()->getSystem<RenderSystem>()->updateWeaponEntitySprite(weapon, dt);

            cooldown->cooldownTimer = cooldown->cooldownDuration;
            AXLOG("Sword %u cooldown reset and starts new cycle. Cooldown: %f", weapon, cooldown->cooldownDuration);
        }
    }
}

void WeaponSystem::initializePlayerWeapon(Entity player)
{
    if (auto weaponInventory = weaponInventoryMgr.getComponent(player))
    {
        std::string defaultWeapon = weaponInventory->weapons[0].first;
        weaponPool.push_back(createSwordEntity(defaultWeapon));
    }
    else
    {
        AXLOG("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz  nulllllllllllllllllllllllllllllllllllll", );
    }
}

void WeaponSystem::upgradeWeapon()
{
    auto weaponInventory = weaponInventoryMgr.getComponent(playerEntity);
    weaponInventory->weapons[0].second = 2; //gán lv 2
    weaponPool.push_back(createSwordEntity("sword")); // tạo thêm kiếm (chỉ lv 2)
}

Entity WeaponSystem::createSwordEntity(std::string weaponName)
{
    const auto& templ = GameData::getInstance()->getEntityTemplates();
    std::string type  = GameData::getInstance()->findTypeByName(templ, weaponName);

    Entity weapon = factory->createEntity(type, weaponName);

    auto* sprite = spriteMgr.getComponent(weapon);
    if (sprite)
    {
        sprite->initializeSprite();
    }

    return weapon;
}


