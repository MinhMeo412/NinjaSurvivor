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
        AXLOG("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz  nulllllllllllllllllllllllllllllllllllll" );
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

