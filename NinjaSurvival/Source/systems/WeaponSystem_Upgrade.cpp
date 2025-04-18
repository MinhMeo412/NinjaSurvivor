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

// Buffs
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
