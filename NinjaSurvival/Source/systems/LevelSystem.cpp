#include "LevelSystem.h"
#include "SystemManager.h"
#include "SpawnSystem.h"
#include "axmol.h"

LevelSystem::LevelSystem(EntityManager& em, ComponentManager<WeaponInventoryComponent>& wim) : entityMgr(em), wiMgr(wim) {}

void LevelSystem::init()
{
    currentXP = 0;
    neededXP  = 10;
    level     = 1;
}

void LevelSystem::update(float dt)
{
    if (currentXP >= neededXP)
    {
        levelUp();

        chooseWeapon();
    }
}

void LevelSystem::levelUp()
{
    level     = level + 1;
    currentXP = currentXP - neededXP;
    neededXP  = 5 * (level + 1);

    // Tính neededXP với hệ số tăng trưởng dựa trên mốc cấp độ
    float growthMultiplier = 1.0f;  // Hệ số mặc định
    if (level >= 30)
    {
        growthMultiplier = 2.0f;  // Tăng gấp đôi độ khó từ cấp 30
    }
    else if (level >= 20)
    {
        growthMultiplier = 1.5f;  // Tăng 50% độ khó từ cấp 20
    }
    else if (level >= 10)
    {
        growthMultiplier = 1.2f;  // Tăng 20% độ khó từ cấp 10
    }

    neededXP = 5 * (level + 1) * growthMultiplier;
    AXLOG("Level: %d", level);
}

void LevelSystem::increaseXP(float xp)
{
    currentXP = currentXP + xp; //thêm buff

    //AXLOG("Xp hiện tại: + %f = %f", xp, currentXP);
}

std::vector<std::string> LevelSystem::upgradeGenerator()
{
    auto spawnSystem = SystemManager::getInstance()->getSystem<SpawnSystem>();
    auto playerEntity = spawnSystem->getPlayerEntity();
    auto invent     = wiMgr.getComponent(playerEntity);
    bool weaponFull = true;
    bool buffFull   = true;
    bool weaponSlotFull = true;
    bool buffSlotFull = true;

    std::vector<std::string> exclude;

    for (int i = 0; i < invent->weapons.size(); i++)
    {
        if (invent->weapons[i].first == "")
        {
            weaponSlotFull = false;
        }

        if (invent->weapons[i].second < 5)
        {
            weaponFull = false;
        }

        if (invent->weapons[i].second == 5)
        {
            exclude.push_back(invent->weapons[i].first);
        }
    }

    for (int i = 0; i < invent->buffs.size(); i++)
    {
        if (invent->buffs[i].first == "")
        {
            buffSlotFull = false;
        }

        if (invent->buffs[i].second < 5)
        {
            buffFull = false;
        }

        if (invent->buffs[i].second == 5)
        {
            exclude.push_back(invent->buffs[i].first);
        }
    }
    
    //Combine 2 arrays
    std::vector<std::string> combinedArray;

    //Exclude the values in 'exclude'
    std::unordered_set<std::string> excludeSet(exclude.begin(), exclude.end());

    if (weaponSlotFull && !buffSlotFull)
    {   
        for (int i = 0; i < invent->weapons.size(); i++)
        {
            combinedArray.push_back(invent->weapons[i].first);
        }
        combinedArray.insert(combinedArray.end(), buffs.begin(), buffs.end());
    }
    else if (!weaponSlotFull && buffSlotFull)
    {
        for (int i = 0; i < invent->buffs.size(); i++)
        {
            combinedArray.push_back(invent->buffs[i].first);
        }
        combinedArray.insert(combinedArray.end(), weapons.begin(), weapons.end());
    }
    else if (!weaponSlotFull && !buffSlotFull)
    {
        combinedArray = weapons;
        combinedArray.insert(combinedArray.end(), buffs.begin(), buffs.end());
    }

    std::vector<std::string> filterUpgrade;

    for (const auto& value : combinedArray)
    {
        if (excludeSet.find(value) == excludeSet.end())
        {
            filterUpgrade.push_back(value);
        }
    }

    // Seed the random number generator
    std::random_device rd;
    std::mt19937 gen(rd());

    // Ensure we don't select the same index more than once
    std::unordered_set<int> selectedIndices;
    std::vector<std::string> randomValues;

    if (!weaponFull || !buffFull)
    {
        // Select 3 unique random indices
        while (selectedIndices.size() < 3)
        {
            std::uniform_int_distribution<> dis(0, filterUpgrade.size() - 1);
            int randomIndex = dis(gen);

            if (selectedIndices.insert(randomIndex).second)
            {
                randomValues.push_back(filterUpgrade[randomIndex]);
            }
        }
        AXLOG("level up: %s, %s, %s", randomValues[0].c_str(), randomValues[1].c_str(), randomValues[2].c_str());

    }
    else
    {
        //select coin or health
        AXLOG("Full Buffs and weapons");
    }

    return randomValues;
}

void LevelSystem::chooseWeapon()
{
    std::vector<std::string> upgradeList = upgradeGenerator();
    // pull up selection layer
    //WeaponSystem::upgradeWeapon
}

