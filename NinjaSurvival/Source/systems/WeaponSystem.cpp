#include "WeaponSystem.h"
#include "EntityFactory.h"
#include "SpawnSystem.h"
#include "RenderSystem.h"
#include "SystemManager.h"
#include "GameData.h"

void WeaponSystem::init()
{
    // Khai báo các kiểu tạo weapon
    createWeapon["sword"] = [this](std::string weaponName) { return createSwordEntity(weaponName); };
    createWeapon["shuriken"] = [this](std::string weaponName) { return createShurikenEntity(weaponName); };

    //Khai báo các kiểu update weapon
    //updateWeapon["sword"] = [this](std::string weaponName) { updateSword(weaponName); };
    //updateWeapon["shuriken"] = [this](std::string weaponName) { updateShuriken(weaponName); };

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

        //Chia update theo từng loại weapon
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
    if(auto weaponInventory      = weaponInventoryMgr.getComponent(player))
    {
        std::string defaultWeapon = weaponInventory->weapons[0].first;
        auto it                   = createWeapon.find(defaultWeapon);
        if (it != createWeapon.end())
        {
            // Gọi hàm khởi tạo tương ứng
            weaponPool.push_back(it->second(defaultWeapon));
            weaponPool.push_back(it->second(defaultWeapon));//Test với 2 nhát chém
        }
    }
    else
    {
        AXLOG("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz  nulllllllllllllllllllllllllllllllllllll", );
    }
}

void WeaponSystem::upgradeWeapon(std::string weaponName)
{
    auto weaponInventory = weaponInventoryMgr.getComponent(playerEntity);
    for (int i = 0; i < weaponInventory->weapons.size(); i++)
    {
        if (weaponInventory->weapons[i].first == weaponName && weaponInventory->weapons[i].second < 5)
        {
            weaponInventory->weapons[i].second += 1;
            //upgradeWeaponStats(weaponInventory->weapons[i].first, weaponInventory->weapons[i].second); (string weaponName, int level)
        }
    }
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

Entity WeaponSystem::createShurikenEntity(std::string weaponName)
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
