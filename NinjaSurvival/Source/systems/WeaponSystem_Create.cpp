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





