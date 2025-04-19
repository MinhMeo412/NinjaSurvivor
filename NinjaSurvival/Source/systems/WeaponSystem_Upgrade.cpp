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

// Weapons
//Sword
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

//Shuriken
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

//Kunai
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

//Big kunai
void WeaponSystem::upgradeBigKunai(std::string weaponName, int level)
{
    if (weaponName != "big_kunai")
        return;

    switch (level)
    {
    case 2:
    {
        for (auto bigKunai : bigKunaiList)
        {
            // Giảm cooldown
            auto cooldown = cooldownMgr.getComponent(bigKunai);
            cooldown->cooldownDuration -= 1.0;
            //Tăng size
            auto transform = transformMgr.getComponent(bigKunai);
            auto hitbox    = hitboxMgr.getComponent(bigKunai);
            transform->scale += 0.25;
            hitbox->defaultSize *= 1.25;
        }
        break;
    }
    case 3:
    {
        for (auto bigKunai : bigKunaiList)
        {
            // Tăng duration
            auto duration = durationMgr.getComponent(bigKunai);
            duration->duration += 1.0;
            // Tăng dame
            auto attack = attackMgr.getComponent(bigKunai);
            attack->baseDamage += 5;
            // Tăng speed
            auto speed = speedMgr.getComponent(bigKunai);
            speed->speed += 100;
        }
        break;
    }
    case 4:
    {
        for (auto bigKunai : bigKunaiList)
        {
            // Tăng duration
            auto duration = durationMgr.getComponent(bigKunai);
            duration->duration += 1.0;
            // Tăng size
            auto transform = transformMgr.getComponent(bigKunai);
            auto hitbox    = hitboxMgr.getComponent(bigKunai);
            transform->scale += 0.25;
            hitbox->defaultSize *= 1.2;
        }
        break;
    }
    case 5:
    {
        for (auto bigKunai : bigKunaiList)
        {
            // Giảm cooldown
            auto cooldown = cooldownMgr.getComponent(bigKunai);
            cooldown->cooldownDuration -= 1.0;
            // Tăng dame
            auto attack = attackMgr.getComponent(bigKunai);
            attack->baseDamage += 5;
        }

        // Thêm 1 big kunai
        auto inventBigKunai = bigKunaiList[0];
        auto it = createWeapon.find(weaponName);
        auto newBigKunai = it->second(weaponName, true);
        weaponPool.push_back(newBigKunai);
        auto cooldown   = cooldownMgr.getComponent(newBigKunai);
        auto duration   = durationMgr.getComponent(newBigKunai);
        auto attack     = attackMgr.getComponent(newBigKunai);
        auto hitbox     = hitboxMgr.getComponent(newBigKunai);
        auto transform  = transformMgr.getComponent(newBigKunai);
        auto speed      = speedMgr.getComponent(newBigKunai);

        auto stCooldown  = cooldownMgr.getComponent(inventBigKunai);
        auto stDuration  = durationMgr.getComponent(inventBigKunai);
        auto stAttack    = attackMgr.getComponent(inventBigKunai);
        auto stHitbox    = hitboxMgr.getComponent(inventBigKunai);
        auto stTransform = transformMgr.getComponent(inventBigKunai);
        auto stSpeed     = speedMgr.getComponent(inventBigKunai);

        cooldown->cooldownDuration = stCooldown->cooldownDuration;
        cooldown->cooldownTimer    = stCooldown->cooldownTimer;
        duration->duration         = stDuration->duration;
        attack->baseDamage         = stAttack->baseDamage;
        hitbox->defaultSize        = stHitbox->defaultSize;
        transform->scale           = stTransform->scale;
        speed->speed               = stSpeed->speed;

        break;
    }
    default:
        break;
    }
}

// Buffs
//Attack
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

//Health
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

//Speed
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

//XPGain
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

//Pickup Range


//Reduce receive damage


//Coin gain


//Recovery


//Curse (increase attack - increase receive damage)


