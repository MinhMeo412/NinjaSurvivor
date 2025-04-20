#include "LevelSystem.h"
#include "ShopSystem.h"
#include "SystemManager.h"
#include "SpawnSystem.h"
#include "gameUI/LevelUpOrChestEventLayer.h"
#include "scenes/GameScene.h"

LevelSystem::LevelSystem(EntityManager& em, ComponentManager<WeaponInventoryComponent>& wim) : entityMgr(em), wiMgr(wim) {}

void LevelSystem::init()
{
    currentXP = 0;
    neededXP  = 5;
    level     = 1;
    rerollCount = static_cast<int>(ShopSystem::getInstance()->getShopBuff("RerollWeapon"));
    shopXpGainBuff = ShopSystem::getInstance()->getShopBuff("XPGain");
    ;  // Lấy thêm từ shop
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

    float growthMultiplier = 1.0f + (0.1 * static_cast<int>(level / 4)); //Tăng 10% mỗi 4 level

    neededXP = 6 * (level + 1) * growthMultiplier;
    AXLOG("Level: %d", level);
}

void LevelSystem::increaseXP(float xp)
{
    float xpMultiplier = 1.0f + shopXpGainBuff + inventXpGainBuff;
    currentXP = currentXP + (xp * xpMultiplier); //thêm buff
}

std::unordered_map<std::string, int> LevelSystem::upgradeGenerator(bool isLevelUp)
{
    auto spawnSystem    = SystemManager::getInstance()->getSystem<SpawnSystem>();
    auto playerEntity   = spawnSystem->getPlayerEntity();
    auto invent         = wiMgr.getComponent(playerEntity);
    bool weaponFull     = true;
    bool buffFull       = true;
    bool weaponSlotFull = true;
    bool buffSlotFull   = true;

    std::vector<std::string> exclude;
    std::unordered_map<std::string, int> lookup;

    // Part 1: check inventory
    for (int i = 0; i < invent->weapons.size(); i++)
    {
        if (invent->weapons[i].first == "")
        {
            weaponSlotFull = false;
        }

        if (invent->weapons[i].second < 5)
        {
            weaponFull                       = false;
            lookup[invent->weapons[i].first] = invent->weapons[i].second;
        }

        if (invent->weapons[i].second == 5)
        {
            exclude.push_back(invent->weapons[i].first);
            lookup[invent->weapons[i].first] = invent->weapons[i].second;
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
            buffFull                       = false;
            lookup[invent->buffs[i].first] = invent->buffs[i].second;
        }

        if (invent->buffs[i].second == 5)
        {
            exclude.push_back(invent->buffs[i].first);
            lookup[invent->buffs[i].first] = invent->buffs[i].second;
        }
    }

    // Part 2: filter the selection base on condition and inventory
    std::vector<std::string> combinedArray;
    std::unordered_set<std::string> excludeSet(exclude.begin(), exclude.end());
    std::vector<std::string> filterUpgrade;

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
    else
    {
        for (int i = 0; i < invent->weapons.size(); i++)
        {
            combinedArray.push_back(invent->weapons[i].first);
        }
        for (int i = 0; i < invent->buffs.size(); i++)
        {
            combinedArray.push_back(invent->buffs[i].first);
        }
    }

    for (const auto& value : combinedArray)
    {
        if (excludeSet.find(value) == excludeSet.end())
        {
            filterUpgrade.push_back(value);
        }
    }

    // Part 3: random generate results
    std::random_device rd;
    std::mt19937 gen(rd());

    std::unordered_set<int> selectedIndices;
    std::vector<std::string> randomValues;

    if (!weaponFull || !buffFull)
    {
        // Select 3 unique random indices
        while (selectedIndices.size() < 3 && selectedIndices.size() < filterUpgrade.size())
        {

            std::uniform_int_distribution<> dis(0, filterUpgrade.size() - 1);
            int randomIndex = dis(gen);

            if (selectedIndices.insert(randomIndex).second)
            {
                randomValues.push_back(filterUpgrade[randomIndex]);
                // AXLOG("RandomValue: %s",.c_str());
            }
        }

        std::vector<std::string> fallback;
        std::unordered_set<std::string> used;
        for (const auto& val : others)
        {
            if (used.count(val) == 0)
            {
                fallback.push_back(val);
            }
        }

        std::shuffle(fallback.begin(), fallback.end(), gen);

        for (const auto& val : fallback)
        {
            if (randomValues.size() == 3)
                break;
            randomValues.push_back(val);
        }
    }
    else
    {
        while (selectedIndices.size() < 3 && selectedIndices.size() < others.size())
        {
            std::uniform_int_distribution<> dis(0, others.size() - 1);
            int randomIndex = dis(gen);

            if (selectedIndices.insert(randomIndex).second)
            {
                randomValues.push_back(others[randomIndex]);
            }
        }
        AXLOG("Full Buffs and Weapons");
    }

    // Part 4: Turn random values generated into unordered_map to return + logic for chest
    std::unordered_map<std::string, int> levelUpResult;
    std::unordered_map<std::string, int> chestResult;
    std::unordered_set<std::string> upgradeSet(filterUpgrade.begin(), filterUpgrade.end());

    if (isLevelUp)
    {
        for (const auto& key : randomValues)
        {
            auto it = lookup.find(key);
            if (it != lookup.end())
            {
                levelUpResult[key] = it->second;
            }
            else
            {
                levelUpResult[key] = 0;
            }
        }

        for (auto& [key, value] : levelUpResult)
        {
            if (upgradeSet.count(key))
            {
                value += 1;
            }
        }

        int i = 1;
        for (const auto& entry : levelUpResult)
        {
            AXLOG("LevelUp upgrade %d: %s, %d", i++, entry.first.c_str(), entry.second);
        }

        return levelUpResult;
    }
    else
    {
        for (const auto& key : randomValues)
        {
            auto it = lookup.find(key);
            if (it != lookup.end())
            {
                chestResult[key] = it->second;
                break;
            }
        }
        if (chestResult.empty())
        {
            chestResult.insert({"coin", 0});
        }

        for (auto& [key, value] : chestResult)
        {
            if (upgradeSet.count(key))
            {
                value += 1;
            }
        }

        for (const auto& entry : chestResult)
        {
            AXLOG("Chest upgrade: %s, %d", entry.first.c_str(), entry.second);
        }

        return chestResult;
    }
}

void LevelSystem::chooseWeapon()
{
    std::unordered_map<std::string, int> upgradeList = upgradeGenerator(true);

    auto gameScene = dynamic_cast<GameScene*>(SystemManager::getInstance()->getCurrentScene());
    if (gameScene)
    {
        auto levelUpLayer = LevelUpOrChestEventLayer::create(true, upgradeList);  // isLevelUp = true
        if (levelUpLayer)
        {
            // Lấy vị trí của uiLayer
            ax::Vec2 uiLayerPos = gameScene->getUILayer()->getPosition();
            levelUpLayer->setPosition(uiLayerPos);
            gameScene->addChild(levelUpLayer, 1000);  // Thêm layer
            //gameScene->unscheduleUpdate();  // Dừng update
            SystemManager::getInstance()->setUpdateState(false);
        }
    }
}

