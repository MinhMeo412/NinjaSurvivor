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

    // Khai báo các kiểu upgrade weapon
    upgradeWeapon["sword"]      = [this](std::string weaponName, int level) { upgradeSword(weaponName, level); };
    upgradeWeapon["shuriken"]   = [this](std::string weaponName, int level) { upgradeShuriken(weaponName, level); };
    upgradeWeapon["kunai"]      = [this](std::string weaponName, int level) { upgradeKunai(weaponName, level); };

    // Khai báo các kiểu upgrade buff
    upgradeBuff["attack"]       = [this](std::string buffName, int level) { upgradeAttack(buffName, level); };
    upgradeBuff["health"]       = [this](std::string buffName, int level) { upgradeHealth(buffName, level); };
    upgradeBuff["speed"]        = [this](std::string buffName, int level) { upgradeSpeed(buffName, level); };
    upgradeBuff["xp_gain"]      = [this](std::string buffName, int level) { upgradeXPGain(buffName, level); };

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
        }
    }
    else
    {
        AXLOG("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz  nulllllllllllllllllllllllllllllllllllll", );
    }
}

void WeaponSystem::upgradeWeaponAndBuff(std::string wpOrBuffName)
{
    if (Utils::not_in(wpOrBuffName, "coin", "heart"))
    {
        const auto& templ = GameData::getInstance()->getEntityTemplates();
        std::string type  = GameData::getInstance()->findTypeByName(templ, wpOrBuffName);

        auto weaponInventory = weaponInventoryMgr.getComponent(playerEntity);

        if (type == "") // Upgrade buff
        {
            bool found = false;
            for (auto& [name, level] : weaponInventory->buffs)
            {
                if (name == wpOrBuffName) //Nếu đã có trong invent
                {
                    level += 1;  // Tăng level trong inventory
                    auto it = upgradeBuff.find(wpOrBuffName);
                    if (it != upgradeBuff.end())
                    {
                        // Gọi hàm upgrade tương ứng
                        it->second(wpOrBuffName, level);
                    }
                    found = true;
                    break;
                }
            }
            if (!found) //Nếu chưa có trong invent
            {
                for (auto& [name, level] : weaponInventory->buffs)
                {
                    if (name == "")
                    {
                        name  = wpOrBuffName;
                        level = 1;
                        auto it = upgradeBuff.find(wpOrBuffName);
                        if (it != upgradeBuff.end())
                        {
                            // Gọi hàm upgrade tương ứng
                            it->second(wpOrBuffName, level);
                        }
                        break;
                    }
                }
            }
        }
        else // Upgrade weapon
        {
            bool found = false;
            for (auto& [name, level] : weaponInventory->weapons)
            {
                if (name == wpOrBuffName) //Nếu đã có trong invent
                {
                    level += 1;  // Tăng level trong inventory
                    auto it = upgradeWeapon.find(wpOrBuffName);
                    if (it != upgradeWeapon.end())
                    {
                        // Gọi hàm upgrade tương ứng
                        it->second(wpOrBuffName, level);
                    }
                    found = true;
                    break;
                }
            }
            if (!found) //Nếu chưa có trong invent
            {
                for (auto& [name, level] : weaponInventory->weapons)
                {
                    if (name == "")
                    {
                        auto it = createWeapon.find(wpOrBuffName);
                        if (it != createWeapon.end())
                        {
                            // Gọi hàm khởi tạo tương ứng
                            weaponPool.push_back(it->second(wpOrBuffName, false));
                        }
                        break;
                    }
                }
            }
        }
    }
    else
    {
        if (wpOrBuffName == "coin")
        {

        }
        else if (wpOrBuffName == "heart")
        {

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

    swordList.push_back(weapon);

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

    shurikenList.push_back(weapon);

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

void WeaponSystem::upgradeSword(std::string weaponName, int level)
{
    if (weaponName != "sword")
        return;

    switch (level)
    {
    case 2:
    {
        // Thêm 1 kiếm chém phía sau
        auto it = createWeapon.find(weaponName);
        weaponPool.push_back(it->second(weaponName, true));
        // Đồng bộ CD
        for (auto sword : swordList)
        {
            auto cooldown = cooldownMgr.getComponent(sword);
            cooldown->cooldownTimer = 0.0;
        }
        break;
    }
    case 3:
    {
        for (auto sword : swordList)
        {
            // Giảm cooldown
            auto cooldown           = cooldownMgr.getComponent(sword);
            cooldown->cooldownDuration -= 0.25;
            // Tăng dame
            auto attack = attackMgr.getComponent(sword);
            attack->baseDamage += 5;
        }
        break;
    }
    case 4:
    {
        // Giảm cooldown
        for (auto sword : swordList)
        {
            auto cooldown = cooldownMgr.getComponent(sword);
            cooldown->cooldownDuration -= 0.25;
        }
        break;
    }
    case 5:
    {
        for (auto sword : swordList)
        {
            // Tăng size
            auto transform   = transformMgr.getComponent(sword);
            transform->scale = 2.0;
            auto hitbox      = hitboxMgr.getComponent(sword);
            hitbox->defaultSize *= 2.0;
            // Tăng dame
            auto attack = attackMgr.getComponent(sword);
            attack->baseDamage += 5;
        }
        break;
    }
    default:
        break;
    }
}

void WeaponSystem::upgradeShuriken(std::string weaponName, int level)
{
    if (weaponName != "shuriken")
        return;

    switch (level)
    {
    case 2:  
    {
        // Thêm 1 shuriken
        auto it = createWeapon.find(weaponName);
        weaponPool.push_back(it->second(weaponName, true));
        SystemManager::getInstance()->getSystem<MovementSystem>()->getWeaponMoveSystem()->recalculateShurikenAngles(shurikenList);
        for (auto shuriken : shurikenList)
        {
            // Đồng bộ CD
            auto cooldown           = cooldownMgr.getComponent(shuriken);
            cooldown->cooldownTimer = 0.0;
        }
        break;
    }
    case 3:
    {
        // Thêm 1 shuriken
        auto it = createWeapon.find(weaponName);
        weaponPool.push_back(it->second(weaponName, true));
        SystemManager::getInstance()->getSystem<MovementSystem>()->getWeaponMoveSystem()->recalculateShurikenAngles(shurikenList);

        auto cooldownSync = cooldownMgr.getComponent(shurikenList[0]);
        // Giảm CD
        cooldownSync->cooldownDuration -= 0.5;
        for (auto shuriken : shurikenList)
        {
            // Đồng bộ CD
            auto cooldown           = cooldownMgr.getComponent(shuriken);
            cooldown->cooldownTimer = 0.0;
            cooldown->cooldownDuration = cooldownSync->cooldownDuration;
        }
        break;
    }
    case 4:
    {
        // Thêm 1 shuriken
        auto it = createWeapon.find(weaponName);
        weaponPool.push_back(it->second(weaponName, true));
        SystemManager::getInstance()->getSystem<MovementSystem>()->getWeaponMoveSystem()->recalculateShurikenAngles(shurikenList);
        auto cooldownSync = cooldownMgr.getComponent(shurikenList[0]);
        // Giảm CD
        cooldownSync->cooldownDuration -= 1.0;
        for (auto shuriken : shurikenList)
        {
            // Đồng bộ CD
            auto cooldown              = cooldownMgr.getComponent(shuriken);
            cooldown->cooldownTimer    = 0.0;
            cooldown->cooldownDuration = cooldownSync->cooldownDuration;
        }
        break;
    }
    case 5: 
    {
        // Thêm 1 shuriken
        auto it = createWeapon.find(weaponName);
        weaponPool.push_back(it->second(weaponName, true));
        SystemManager::getInstance()->getSystem<MovementSystem>()->getWeaponMoveSystem()->recalculateShurikenAngles(shurikenList);
        for (auto shuriken : shurikenList)
        {
            // Tăng size
            auto transform   = transformMgr.getComponent(shuriken);
            transform->scale = 2.0;
            auto hitbox      = hitboxMgr.getComponent(shuriken);
            hitbox->defaultSize *= 2.0;
            // Tăng dame
            auto attack = attackMgr.getComponent(shuriken);
            attack->baseDamage += 5;
        }
        break;
    }
    default:
        break;
    }
}

void WeaponSystem::upgradeKunai(std::string weaponName, int level)
{
    if (weaponName != "kunai")
        return;

    switch (level)
    {
    case 2:
    {
        // Giảm CD
        auto cooldown = cooldownMgr.getComponent(kunaiEntity);
        cooldown->cooldownDuration -= 0.10;
        break;
    }
    case 3:
    {
        // Giảm CD
        auto cooldown = cooldownMgr.getComponent(kunaiEntity);
        cooldown->cooldownDuration -= 0.15;
        // Tăng dame
        auto attack = attackMgr.getComponent(kunaiEntity);
        attack->baseDamage += 2;
        break;
    }
    case 4:
    {
        // Giảm CD
        auto cooldown = cooldownMgr.getComponent(kunaiEntity);
        cooldown->cooldownDuration -= 0.15;
        break;
    }
    case 5:
    {
        // Giảm CD
        auto cooldown = cooldownMgr.getComponent(kunaiEntity);
        cooldown->cooldownDuration -= 0.25;
        // Tăng dame
        auto attack = attackMgr.getComponent(kunaiEntity);
        attack->baseDamage += 3;
        break;
    }
    default:
        break;
    }
}

void WeaponSystem::upgradeAttack(std::string buffName, int level)
{
    if (buffName != "attack")
        return;

    switch (level)
    {
    case 1:
    {
        // Tăng 10%
        attackBuff += 0.1;
        break;
    }
    case 2:
    {
        // Tăng 10%
        attackBuff += 0.1;
        break;
    }
    case 3:
    {
        // Tăng 10%
        attackBuff += 0.1;
        break;
    }
    case 4:
    {
        // Tăng 10%
        attackBuff += 0.1;
        break;
    }
    case 5:
    {
        // Tăng 10%
        attackBuff += 0.1;
        break;
    }
    default:
        break;
    }
}

void WeaponSystem::upgradeHealth(std::string buffName, int level)
{
    if (buffName != "health")
        return;

    auto identity      = identityMgr.getComponent(playerEntity);
    auto characterData = GameData::getInstance()->getEntityTemplates().at(identity->type).at(identity->name);
    float baseHealth   = characterData.health->maxHealth;

    switch (level)
    {
    case 1:
    {
        // Tăng 10%
        float healthMultiplier = 0.1;
        float hpIncrease        = (baseHealth * (1 + healthMultiplier)) - baseHealth;

        auto health     = healthMgr.getComponent(playerEntity);
        health->maxHealth += hpIncrease;
        health->currentHealth += hpIncrease;
        if (health->currentHealth > health->maxHealth)
        {
            health->currentHealth = health->maxHealth;
        }
        break;
    }
    case 2:
    {
        // Tăng 10%
        float healthMultiplier = 0.1;
        float hpIncrease       = (baseHealth * (1 + healthMultiplier)) - baseHealth;

        auto health = healthMgr.getComponent(playerEntity);
        health->maxHealth += hpIncrease;
        health->currentHealth += hpIncrease;
        if (health->currentHealth > health->maxHealth)
        {
            health->currentHealth = health->maxHealth;
        }
        break;
    }
    case 3:
    {
        // Tăng 10%
        float healthMultiplier = 0.1;
        float hpIncrease       = (baseHealth * (1 + healthMultiplier)) - baseHealth;

        auto health = healthMgr.getComponent(playerEntity);
        health->maxHealth += hpIncrease;
        health->currentHealth += hpIncrease;
        if (health->currentHealth > health->maxHealth)
        {
            health->currentHealth = health->maxHealth;
        }
        break;
    }
    case 4:
    {
        // Tăng 10%
        float healthMultiplier = 0.1;
        float hpIncrease       = (baseHealth * (1 + healthMultiplier)) - baseHealth;

        auto health = healthMgr.getComponent(playerEntity);
        health->maxHealth += hpIncrease;
        health->currentHealth += hpIncrease;
        if (health->currentHealth > health->maxHealth)
        {
            health->currentHealth = health->maxHealth;
        }
        break;
    }
    case 5:
    {
        // Tăng 10%
        float healthMultiplier = 0.1;
        float hpIncrease       = (baseHealth * (1 + healthMultiplier)) - baseHealth;

        auto health = healthMgr.getComponent(playerEntity);
        health->maxHealth += hpIncrease;
        health->currentHealth += hpIncrease;
        if (health->currentHealth > health->maxHealth)
        {
            health->currentHealth = health->maxHealth;
        }
        break;
    }
    default:
        break;
    }
}

void WeaponSystem::upgradeSpeed(std::string buffName, int level)
{
    if (buffName != "speed")
        return;

    auto identity      = identityMgr.getComponent(playerEntity);
    auto characterData = GameData::getInstance()->getEntityTemplates().at(identity->type).at(identity->name);
    float baseSpeed   = characterData.speed->speed;

    switch (level)
    {
    case 1:
    {
        // Tăng 10%
        float speedMultiplier = 0.1;
        float speedIncrease   = (baseSpeed * (1 + speedMultiplier)) - baseSpeed;

        auto speed = speedMgr.getComponent(playerEntity);
        speed->speed += speedIncrease;

        break;
    }
    case 2:
    {
        // Tăng 10%
        float speedMultiplier = 0.1;
        float speedIncrease   = (baseSpeed * (1 + speedMultiplier)) - baseSpeed;

        auto speed = speedMgr.getComponent(playerEntity);
        speed->speed += speedIncrease;

        break;
    }
    case 3:
    {
        // Tăng 10%
        float speedMultiplier = 0.1;
        float speedIncrease   = (baseSpeed * (1 + speedMultiplier)) - baseSpeed;

        auto speed = speedMgr.getComponent(playerEntity);
        speed->speed += speedIncrease;

        break;
    }
    case 4:
    {
        // Tăng 10%
        float speedMultiplier = 0.1;
        float speedIncrease   = (baseSpeed * (1 + speedMultiplier)) - baseSpeed;

        auto speed = speedMgr.getComponent(playerEntity);
        speed->speed += speedIncrease;

        break;
    }
    case 5:
    {
        // Tăng 10%
        float speedMultiplier = 0.1;
        float speedIncrease   = (baseSpeed * (1 + speedMultiplier)) - baseSpeed;

        auto speed = speedMgr.getComponent(playerEntity);
        speed->speed += speedIncrease;

        break;
    }
    default:
        break;
    }
}

void WeaponSystem::upgradeXPGain(std::string buffName, int level)
{
    if (buffName != "xp_gain")
        return;

    switch (level)
    {
    case 1:
    {
        // Tăng 10%
        SystemManager::getInstance()->getSystem<LevelSystem>()->inventXpGainBuff += 0.1;
        break;
    }
    case 2:
    {
        // Tăng 10%
        SystemManager::getInstance()->getSystem<LevelSystem>()->inventXpGainBuff += 0.1;
        break;
    }
    case 3:
    {
        // Tăng 10%
        SystemManager::getInstance()->getSystem<LevelSystem>()->inventXpGainBuff += 0.1;
        break;
    }
    case 4:
    {
        // Tăng 10%
        SystemManager::getInstance()->getSystem<LevelSystem>()->inventXpGainBuff += 0.1;
        break;
    }
    case 5:
    {
        // Tăng 10%
        SystemManager::getInstance()->getSystem<LevelSystem>()->inventXpGainBuff += 0.1;
        break;
    }
    default:
        break;
    }
}
