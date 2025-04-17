#include "WeaponSystem.h"
#include "EntityFactory.h"
#include "SpawnSystem.h"
#include "TimeSystem.h"
#include "RenderSystem.h"
#include "SystemManager.h"
#include "GameData.h"

void WeaponSystem::init()
{
    // Khai báo các kiểu tạo weapon
    createWeapon["sword"]       = [this](std::string weaponName, bool alreadyHave) { return createSword(weaponName, alreadyHave); };
    createWeapon["shuriken"]    = [this](std::string weaponName, bool alreadyHave) { return createShuriken(weaponName, alreadyHave); };
    createWeapon["kunai"]       = [this](std::string weaponName, bool alreadyHave) { return createKunai(weaponName, alreadyHave); };

    // Khai báo các kiểu update weapon
    updateWeapon["sword"]       = [this](Entity weapon, float dt) { updateSword(weapon, dt); };
    updateWeapon["shuriken"]    = [this](Entity weapon, float dt) { updateShuriken(weapon, dt); };
    updateWeapon["kunai"]       = [this](Entity weapon, float dt) { updateKunai(weapon, dt); };

    // Khởi tạo entity factory
    factory = std::make_unique<EntityFactory>(entityManager, identityMgr, transformMgr, spriteMgr, animationMgr,
                                              velocityMgr, hitboxMgr, healthMgr, attackMgr, cooldownMgr, speedMgr, weaponInventoryMgr, durationMgr);

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
        // Xác định kiểu update
        if (auto identity = identityMgr.getComponent(weapon))
        {
            auto it = updateWeapon.find(identity->name);
            if (it != updateWeapon.end())
            {
                // Gọi hàm update tương ứng
                it->second(weapon,dt);
            }
        }

        //Chạy timer cho các weapon có duration (tgian đến lúc gây dame tiếp theo)
        if (auto duration = durationMgr.getComponent(weapon))
        {
            std::vector<unsigned int> toRemove;
            for (auto& [entity, timeLeft] : duration->timer)
            {
                timeLeft -= dt;
                if (timeLeft <= 0.0f)
                    toRemove.push_back(entity); // Cho vào vector để xóa sau tránh thay đổi map trong lúc đang duyệt loop
            }

            //Chạy loop xóa những entity đã hết tgian chờ
            for (auto entity : toRemove)
            {
                duration->timer.erase(entity);
            }
        }
    }

    //Các weapon trong temp sẽ có 1 bản mẫu trong weaponPool để tham khảo
    for (auto& weapon : tempWeaponPool)
    {
        // Xác định kiểu update
        if (auto identity = identityMgr.getComponent(weapon))
        {
            auto it = updateWeapon.find(identity->name);
            if (it != updateWeapon.end())
            {
                // Gọi hàm update tương ứng
                it->second(weapon, dt);
            }
        }
    }
}

void WeaponSystem::initializePlayerWeapon(Entity player)
{
    if (auto weaponInventory = weaponInventoryMgr.getComponent(player))
    {
        std::string defaultWeapon = weaponInventory->weapons[0].first;
        auto it                   = createWeapon.find(defaultWeapon);
        if (it != createWeapon.end())
        {
            // Gọi hàm khởi tạo tương ứng
            weaponPool.push_back(it->second(defaultWeapon, true));
            weaponPool.push_back(createShuriken("shuriken", false));
            weaponPool.push_back(createShuriken("shuriken", true));
            weaponPool.push_back(createShuriken("shuriken", true));
            weaponPool.push_back(createShuriken("shuriken", true));
            weaponPool.push_back(createShuriken("shuriken", true));
            weaponPool.push_back(createSword("sword", false));
            weaponPool.push_back(createSword("sword", true));
        }
    }
    else
    {
        AXLOG("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz  nulllllllllllllllllllllllllllllllllllll" );
    }
}

void WeaponSystem::upgradeWeapon(std::string weaponName)
{
    auto weaponInventory = weaponInventoryMgr.getComponent(playerEntity);
    for (int i = 0; i < weaponInventory->weapons.size(); i++)
    {
        if (weaponInventory->weapons[i].first == weaponName && weaponInventory->weapons[i].second < 5)
        {
            weaponInventory->weapons[i].second += 1;  // Tăng level trong inventory
            // Tăng stats hoặc các chức năng khác
            // upgradeWeaponStats(weaponInventory->weapons[i].first, weaponInventory->weapons[i].second); (string
            // weaponName, int level)
        }
    }
}

Entity WeaponSystem::createSword(std::string weaponName, bool alreadyHave)
{
    if (!alreadyHave)
    {
        auto weaponInventory = weaponInventoryMgr.getComponent(playerEntity);
        auto it              = std::find_if(weaponInventory->weapons.begin(), weaponInventory->weapons.end(),
                                            [](auto& p) { return p.first == ""; });
        if (it != weaponInventory->weapons.end())
        {
            it->first  = "sword";
            it->second = 1;
        }
    }
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

Entity WeaponSystem::createShuriken(std::string weaponName, bool alreadyHave)
{
    if (!alreadyHave)
    {
        auto weaponInventory = weaponInventoryMgr.getComponent(playerEntity);
        auto it              = std::find_if(weaponInventory->weapons.begin(), weaponInventory->weapons.end(),
                                            [](auto& p) { return p.first == ""; });
        if (it != weaponInventory->weapons.end())
        {
            it->first  = "shuriken";
            it->second = 5;
        }
    }
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

// Chỉ tạo mẫu để đưa vào pool làm tham khảo
Entity WeaponSystem::createKunai(std::string weaponName, bool alreadyHave)
{
    if (!alreadyHave)
    {
        auto weaponInventory = weaponInventoryMgr.getComponent(playerEntity);
        auto it              = std::find_if(weaponInventory->weapons.begin(), weaponInventory->weapons.end(),
                                            [](auto& p) { return p.first == ""; });
        if (it != weaponInventory->weapons.end())
        {
            it->first  = "kunai";
            it->second = 1;
        }
    }
    const auto& templ = GameData::getInstance()->getEntityTemplates();
    std::string type  = GameData::getInstance()->findTypeByName(templ, weaponName);

    Entity weapon = factory->createEntity(type, weaponName);

    auto* sprite = spriteMgr.getComponent(weapon);
    if (sprite)
    {
        sprite->initializeSprite();
    }

    // Không va chạm
    auto* hitbox = hitboxMgr.getComponent(weapon);
    if (hitbox)
    {
        hitbox->size = ax::Size(0, 0);
    }

    // Gán làm kunai mẫu
    kunaiEntity = weapon;
    return weapon;
}

Entity WeaponSystem::createTempKunai(std::string weaponName) 
{
    const auto& templ = GameData::getInstance()->getEntityTemplates();
    std::string type  = GameData::getInstance()->findTypeByName(templ, weaponName);

    Entity weapon = factory->createEntity(type, weaponName);

    auto* sprite = spriteMgr.getComponent(weapon);
    if (sprite)
    {
        sprite->initializeSprite();
    }

    auto* cooldown = cooldownMgr.getComponent(weapon);
    auto* duration = durationMgr.getComponent(weapon);
    cooldown->cooldownTimer = duration->duration;

    auto* transform = transformMgr.getComponent(weapon);
    transform->x   = transformMgr.getComponent(playerEntity)->x + (rand() % 11 - 5);
    transform->y   = transformMgr.getComponent(playerEntity)->y + (rand() % 11 - 5);

    auto* vel = velocityMgr.getComponent(weapon);
    vel->vx  = velocityMgr.getComponent(kunaiEntity)->vx;
    vel->vy  = velocityMgr.getComponent(kunaiEntity)->vy;

    return weapon;
}

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
