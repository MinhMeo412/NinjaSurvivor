#include "Utils.h"
#include "WeaponSystem.h"
#include "EntityFactory.h"
#include "SpawnSystem.h"
#include "MovementSystem.h"
#include "TimeSystem.h"
#include "LevelSystem.h"
#include "HealthSystem.h"
#include "RenderSystem.h"
#include "SystemManager.h"
#include "GameData.h"
#include "gameUI/GameSceneUILayer.h"

void WeaponSystem::init()
{
    // Khai báo các kiểu tạo weapon
    createWeapon["sword"]           = [this](std::string weaponName, bool alreadyHave) { return createSword(weaponName, alreadyHave); };
    createWeapon["shuriken"]        = [this](std::string weaponName, bool alreadyHave) { return createShuriken(weaponName, alreadyHave); };
    createWeapon["kunai"]           = [this](std::string weaponName, bool alreadyHave) { return createKunai(weaponName, alreadyHave); };
    createWeapon["big_kunai"]       = [this](std::string weaponName, bool alreadyHave) { return createBigKunai(weaponName, alreadyHave); };
    createWeapon["spinner"]         = [this](std::string weaponName, bool alreadyHave) { return createSpinner(weaponName, alreadyHave); };
    createWeapon["explosion_kunai"] = [this](std::string weaponName, bool alreadyHave) { return createExplosionKunai(weaponName, alreadyHave); };
    createWeapon["ninjutsu_spell"]  = [this](std::string weaponName, bool alreadyHave) { return createNinjutsuSpell(weaponName, alreadyHave); };
    createWeapon["lightning_scroll"]= [this](std::string weaponName, bool alreadyHave) { return createLightningScroll(weaponName, alreadyHave); };

    // Khai báo các kiểu update weapon
    updateWeapon["energy_ball"]     = [this](Entity weapon, float dt) { updateEnemyProjectile(weapon, dt); };

    updateWeapon["sword"]           = [this](Entity weapon, float dt) { updateSword(weapon, dt); };
    updateWeapon["shuriken"]        = [this](Entity weapon, float dt) { updateShuriken(weapon, dt); };
    updateWeapon["kunai"]           = [this](Entity weapon, float dt) { updateKunai(weapon, dt); };
    updateWeapon["big_kunai"]       = [this](Entity weapon, float dt) { updateBigKunai(weapon, dt); };
    updateWeapon["spinner"]         = [this](Entity weapon, float dt) { updateSpinner(weapon, dt); };
    updateWeapon["explosion_kunai"] = [this](Entity weapon, float dt) { updateExplosionKunai(weapon, dt); };
    updateWeapon["ninjutsu_spell"]  = [this](Entity weapon, float dt) { updateNinjutsuSpell(weapon, dt); };
    updateWeapon["lightning_scroll"]= [this](Entity weapon, float dt) { updateLightningScroll(weapon, dt); };


    // Khai báo các kiểu upgrade weapon
    upgradeWeapon["sword"]          = [this](std::string weaponName, int level) { upgradeSword(weaponName, level); };
    upgradeWeapon["shuriken"]       = [this](std::string weaponName, int level) { upgradeShuriken(weaponName, level); };
    upgradeWeapon["kunai"]          = [this](std::string weaponName, int level) { upgradeKunai(weaponName, level); };
    upgradeWeapon["big_kunai"]      = [this](std::string weaponName, int level) { upgradeBigKunai(weaponName, level); };
    upgradeWeapon["spinner"]        = [this](std::string weaponName, int level) { upgradeSpinner(weaponName, level); };
    upgradeWeapon["explosion_kunai"]= [this](std::string weaponName, int level) { upgradeExplosionKunai(weaponName, level); };
    upgradeWeapon["ninjutsu_spell"] = [this](std::string weaponName, int level) { upgradeNinjutsuSpell(weaponName, level); };
    upgradeWeapon["lightning_scroll"] = [this](std::string weaponName, int level) { upgradeLightningScroll(weaponName, level); };

    // Khai báo các kiểu upgrade buff
    upgradeBuff["attack"]           = [this](std::string buffName, int level) { upgradeAttack(buffName, level); };
    upgradeBuff["health"]           = [this](std::string buffName, int level) { upgradeHealth(buffName, level); };
    upgradeBuff["speed"]            = [this](std::string buffName, int level) { upgradeSpeed(buffName, level); };
    upgradeBuff["xp_gain"]          = [this](std::string buffName, int level) { upgradeXPGain(buffName, level); };
    upgradeBuff["pickup_range"]     = [this](std::string buffName, int level) { upgradePickupRange(buffName, level); };
    upgradeBuff["reduce_receive_damage"] = [this](std::string buffName, int level) { upgradeReduceReceiveDamage(buffName, level); };
    upgradeBuff["coin_gain"]        = [this](std::string buffName, int level) { upgradeCoinGain(buffName, level); };
    upgradeBuff["curse"]            = [this](std::string buffName, int level) { upgradeCurse(buffName, level); };

    // Khởi tạo entity factory
    factory = std::make_unique<EntityFactory>(entityManager, identityMgr, transformMgr, spriteMgr, animationMgr,
                                              velocityMgr, hitboxMgr, healthMgr, attackMgr, cooldownMgr, speedMgr, weaponInventoryMgr, durationMgr);

    auto spawnSystem = SystemManager::getInstance()->getSystem<SpawnSystem>();
    if (spawnSystem)
    {
        playerEntity = spawnSystem->getPlayerEntity();
    }

    initializePlayerWeapon(playerEntity);

    dynamic_cast<GameSceneUILayer*>(SystemManager::getInstance()->getSceneLayer())->updateInventoryUI();
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

        dynamic_cast<GameSceneUILayer*>(SystemManager::getInstance()->getSceneLayer())->updateInventoryUI();
    }
    else
    {
        if (wpOrBuffName == "coin")
        {
            auto gameLayer = dynamic_cast<GameSceneUILayer*>(SystemManager::getInstance()->getSceneLayer());
            gameLayer->increaseCoin(25);
        }
        else if (wpOrBuffName == "heart")
        {
            SystemManager::getInstance()->getSystem<HealthSystem>()->setPlayerCurrentHealth(25.0);
        }
    }
}

std::vector<std::string> WeaponSystem::getPlayerInventory(bool isWp)
{
    auto inventory = weaponInventoryMgr.getComponent(playerEntity);
    if (isWp)
    {
        std::vector<std::string> wpList;
        auto inventWPList = inventory->weapons;
        for (auto& wp : inventWPList)
        {
            wpList.push_back(wp.first);
        }
        return wpList;
    }
    else
    {
        std::vector<std::string> buffList;
        auto inventBuffList = inventory->buffs;
        for (auto& buff : inventBuffList)
        {
            buffList.push_back(buff.first);
        }
        return buffList;
    }
}

