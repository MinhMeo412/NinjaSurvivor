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

//Enemy projectile
Entity WeaponSystem::createEnemyProjectile(std::string weaponName, Entity attacker)
{
    const auto& templ = GameData::getInstance()->getEntityTemplates();
    std::string type  = GameData::getInstance()->findTypeByName(templ, weaponName);

    Entity weapon = factory->createEntity(type, weaponName);

    auto sprite = spriteMgr.getComponent(weapon);
    if (sprite)
    {
        sprite->initializeSprite();
    }

    auto trans = transformMgr.getComponent(weapon);
    auto velocity   = velocityMgr.getComponent(weapon);
    auto duration   = durationMgr.getComponent(weapon);
    auto attack   = attackMgr.getComponent(weapon);

    trans->x = transformMgr.getComponent(attacker)->x;
    trans->y = transformMgr.getComponent(attacker)->y;

    attack->baseDamage = attackMgr.getComponent(attacker)->baseDamage;

    // Tính vector hướng từ attacker đến player
    auto playerTrans = transformMgr.getComponent(playerEntity);
    ax::Vec2 vel = ax::Vec2(playerTrans->x - trans->x, playerTrans->y - trans->y).getNormalized();
    velocity->vx = vel.x;
    velocity->vy = vel.y;

    tempWeaponPool.push_back(weapon);

    return weapon;
}

//Sword
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

    auto sprite = spriteMgr.getComponent(weapon);
    if (sprite)
    {
        sprite->initializeSprite();
    }

    auto cooldown           = cooldownMgr.getComponent(weapon);
    cooldown->cooldownTimer = 1.0f;

    swordList.push_back(weapon);

    return weapon;
}

//Shuriken
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

    auto sprite = spriteMgr.getComponent(weapon);
    if (sprite)
    {
        sprite->initializeSprite();
    }

    auto cooldown           = cooldownMgr.getComponent(weapon);
    cooldown->cooldownTimer = 1.0f;

    shurikenList.push_back(weapon);

    return weapon;
}

//Kunai - Chỉ tạo mẫu để đưa vào pool làm tham khảo
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

    auto sprite = spriteMgr.getComponent(weapon);
    if (sprite)
    {
        sprite->initializeSprite();
    }

    // Không va chạm
    auto hitbox = hitboxMgr.getComponent(weapon);
    if (hitbox)
    {
        hitbox->size = ax::Size(0, 0);
    }

    auto cooldown           = cooldownMgr.getComponent(weapon);
    cooldown->cooldownTimer = 1.0f;

    // Gán làm kunai mẫu
    kunaiEntity = weapon;
    return weapon;
}

//Sub func cho Kunai
Entity WeaponSystem::createTempKunai(std::string weaponName) 
{
    const auto& templ = GameData::getInstance()->getEntityTemplates();
    std::string type  = GameData::getInstance()->findTypeByName(templ, weaponName);

    Entity weapon = factory->createEntity(type, weaponName);

    auto sprite = spriteMgr.getComponent(weapon);
    if (sprite)
    {
        sprite->initializeSprite();
    }

    auto cooldown = cooldownMgr.getComponent(weapon);
    auto duration = durationMgr.getComponent(weapon);
    cooldown->cooldownTimer = duration->duration;

    auto transform = transformMgr.getComponent(weapon);
    transform->x   = transformMgr.getComponent(playerEntity)->x + (rand() % 11 - 5);
    transform->y   = transformMgr.getComponent(playerEntity)->y + (rand() % 11 - 5);

    auto vel = velocityMgr.getComponent(weapon);
    vel->vx  = velocityMgr.getComponent(kunaiEntity)->vx;
    vel->vy  = velocityMgr.getComponent(kunaiEntity)->vy;

    return weapon;
}

//Big Kunai
Entity WeaponSystem::createBigKunai(std::string weaponName, bool alreadyHave)
{
    if (!alreadyHave)
    {
        auto weaponInventory = weaponInventoryMgr.getComponent(playerEntity);
        auto it              = std::find_if(weaponInventory->weapons.begin(), weaponInventory->weapons.end(),
                                            [](auto& p) { return p.first == ""; });
        if (it != weaponInventory->weapons.end())
        {
            it->first  = "big_kunai";
            it->second = 1;
        }
    }
    const auto& templ = GameData::getInstance()->getEntityTemplates();
    std::string type  = GameData::getInstance()->findTypeByName(templ, weaponName);

    Entity weapon = factory->createEntity(type, weaponName);

    auto sprite = spriteMgr.getComponent(weapon);
    if (sprite)
    {
        sprite->initializeSprite();
    }

    auto cooldown          = cooldownMgr.getComponent(weapon);
    cooldown->cooldownTimer = 1.0f;

    bigKunaiList.push_back(weapon);

    return weapon;
}

// Spinner
Entity WeaponSystem::createSpinner(std::string weaponName, bool alreadyHave)
{
    if (!alreadyHave)
    {
        auto weaponInventory = weaponInventoryMgr.getComponent(playerEntity);
        auto it              = std::find_if(weaponInventory->weapons.begin(), weaponInventory->weapons.end(),
                                            [](auto& p) { return p.first == ""; });
        if (it != weaponInventory->weapons.end())
        {
            it->first  = "spinner";
            it->second = 1;
        }
    }
    const auto& templ = GameData::getInstance()->getEntityTemplates();
    std::string type  = GameData::getInstance()->findTypeByName(templ, weaponName);

    Entity weapon = factory->createEntity(type, weaponName);

    auto sprite = spriteMgr.getComponent(weapon);
    if (sprite)
    {
        sprite->initializeSprite();
    }

    auto cooldown           = cooldownMgr.getComponent(weapon);
    cooldown->cooldownTimer = 1.0f;

    spinnerList.push_back(weapon);

    return weapon;
}

// Explosion Kunai
Entity WeaponSystem::createExplosionKunai(std::string weaponName, bool alreadyHave)
{
    if (!alreadyHave)
    {
        auto weaponInventory = weaponInventoryMgr.getComponent(playerEntity);
        auto it              = std::find_if(weaponInventory->weapons.begin(), weaponInventory->weapons.end(),
                                            [](auto& p) { return p.first == ""; });
        if (it != weaponInventory->weapons.end())
        {
            it->first  = "explosion_kunai";
            it->second = 1;
        }
    }
    const auto& templ = GameData::getInstance()->getEntityTemplates();
    std::string type  = GameData::getInstance()->findTypeByName(templ, weaponName);

    Entity weapon = factory->createEntity(type, weaponName);

    auto sprite = spriteMgr.getComponent(weapon);
    if (sprite)
    {
        sprite->initializeSprite();
    }

    auto cooldown           = cooldownMgr.getComponent(weapon);
    cooldown->cooldownTimer = 1.0f;

    explosionKunaiList.push_back(weapon);

    return weapon;
}

// Ninjutsu Spell
Entity WeaponSystem::createNinjutsuSpell(std::string weaponName, bool alreadyHave)
{
    if (!alreadyHave)
    {
        auto weaponInventory = weaponInventoryMgr.getComponent(playerEntity);
        auto it              = std::find_if(weaponInventory->weapons.begin(), weaponInventory->weapons.end(),
                                            [](auto& p) { return p.first == ""; });
        if (it != weaponInventory->weapons.end())
        {
            it->first  = "ninjutsu_spell";
            it->second = 1;
        }
    }
    const auto& templ = GameData::getInstance()->getEntityTemplates();
    std::string type  = GameData::getInstance()->findTypeByName(templ, weaponName);

    Entity weapon = factory->createEntity(type, weaponName);

    auto sprite = spriteMgr.getComponent(weapon);
    if (sprite)
    {
        sprite->initializeSprite();
    }

    auto cooldown           = cooldownMgr.getComponent(weapon);
    cooldown->cooldownTimer = 1.0f;

    ninjutsuSpellList.push_back(weapon);

    return weapon;
}

// Lightning scroll
Entity WeaponSystem::createLightningScroll(std::string weaponName, bool alreadyHave)
{
    if (!alreadyHave)
    {
        auto weaponInventory = weaponInventoryMgr.getComponent(playerEntity);
        auto it              = std::find_if(weaponInventory->weapons.begin(), weaponInventory->weapons.end(),
                                            [](auto& p) { return p.first == ""; });
        if (it != weaponInventory->weapons.end())
        {
            it->first  = "lightning_scroll";
            it->second = 1;
        }
    }
    const auto& templ = GameData::getInstance()->getEntityTemplates();
    std::string type  = GameData::getInstance()->findTypeByName(templ, weaponName);

    Entity weapon = factory->createEntity(type, weaponName);

    auto sprite = spriteMgr.getComponent(weapon);
    if (sprite)
    {
        sprite->initializeSprite();
    }

    auto animation = animationMgr.getComponent(weapon);
    if (animation)
    {
        animation->initializeAnimations();
        animation->currentState = "idle";
    }

    auto cooldown           = cooldownMgr.getComponent(weapon);
    cooldown->cooldownTimer = 1.0f;

    lightningScrollList.push_back(weapon);

    return weapon;
}
