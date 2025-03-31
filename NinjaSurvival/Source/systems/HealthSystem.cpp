#include "HealthSystem.h"
#include "SystemManager.h"
#include "CameraSystem.h"
#include "SpawnSystem.h"
#include "ItemSystem.h"
#include "gameUI/GameOverGamePauseLayer.h"
#include "scenes/GameScene.h"

using namespace ax;

HealthSystem::HealthSystem(EntityManager& em,
                           ComponentManager<IdentityComponent>& im,
                           ComponentManager<HealthComponent>& hm,
                           ComponentManager<AttackComponent>& am,
                           ComponentManager<CooldownComponent>& cm)
    : entityManager(em), identityMgr(im), healthMgr(hm), attackMgr(am), cooldownMgr(cm)
{}

void HealthSystem::init()
{
    // set callback trong init
    onPlayerOutOfHealth = [this]() {
        auto gameScene = dynamic_cast<GameScene*>(SystemManager::getInstance()->getCurrentScene());
        if (gameScene)
        {
            auto pauseLayer = GameOverGamePauseLayer::create(true);  // isPlayerDead = true
            if (pauseLayer)
            {
                // Lấy vị trí của uiLayer
                ax::Vec2 uiLayerPos = gameScene->getUILayer()->getPosition();
                pauseLayer->setPosition(uiLayerPos);
                gameScene->addChild(pauseLayer, 1000);  // Thêm layer
                gameScene->unscheduleUpdate();          // Dừng update
            }
        }
    };
}

void HealthSystem::update(float dt)
{
    // Chạy cooldown cho tất cả entity có CooldownComponent
    for (Entity entity : entityManager.getActiveEntities())
    {
        if (auto cooldown = cooldownMgr.getComponent(entity))
        {
            if (cooldown->cooldownTimer > 0.0f)
            {
                cooldown->cooldownTimer -= dt;
                if (cooldown->cooldownTimer < 0.0f)
                {
                    cooldown->cooldownTimer = 0.0f;
                }
            }
        }
    }
}

void HealthSystem::handleCollision(Entity e1, Entity e2)
{
    //Kiểm tra identity component và lấy type (nếu có)
    auto id1 = identityMgr.getComponent(e1);
    auto id2 = identityMgr.getComponent(e2);
    if (!id1 || !id2)
        return;
    std::string type1 = id1->type;
    std::string type2 = id2->type;

    // Xử lý va chạm giữa enemy/enemy_projectile/boss với player
    if ((type1 == "enemy" || type1 == "enemy_projectile" || type1 == "boss") && type2 == "player")
    {
        if (auto attack = attackMgr.getComponent(e1))
        {
            if (canDealDamage(e1))
            {
                if (auto health = healthMgr.getComponent(e2))
                {
                    float damage = calculateDamage(attack);
                    applyDamage(e2, damage);
                    resetCooldown(e1);
                    AXLOG("Player %u took %f damage from %s %u. HP left: %f", e2, damage, type1.c_str(), e1,
                          health->currentHealth);
                }
            }
        }
    }
    else if ((type2 == "enemy" || type2 == "enemy_projectile" || type2 == "boss") && type1 == "player")
    {
        if (auto attack = attackMgr.getComponent(e2))
        {
            if (canDealDamage(e2))
            {
                if (auto health = healthMgr.getComponent(e1))
                {
                    float damage = calculateDamage(attack);
                    applyDamage(e1, damage);
                    resetCooldown(e2);
                    AXLOG("Player %u took %f damage from %s %u. HP left: %f", e1, damage, type2.c_str(), e2,
                          health->currentHealth);
                }
            }
        }
    }

    // Xử lý va chạm giữa weapon_melee/weapon_projectile với enemy/boss
    if ((type1 == "weapon_melee" || type1 == "weapon_projectile") && (type2 == "enemy" || type2 == "boss"))
    {
        if (auto attack = attackMgr.getComponent(e1))
        {
            if (canDealDamage(e1))
            {
                if (auto health = healthMgr.getComponent(e2))
                {
                    float damage = calculateDamage(attack);
                    applyDamage(e2, damage);
                    resetCooldown(e1);
                    AXLOG("%s %u took %f damage from weapon %u. HP left: %f", type2.c_str(), e2, damage, e1,
                          health->currentHealth);
                }
            }
        }
    }
    else if ((type2 == "weapon_melee" || type2 == "weapon_projectile") && (type1 == "enemy" || type1 == "boss"))
    {
        if (auto attack = attackMgr.getComponent(e2))
        {
            if (canDealDamage(e2))
            {
                if (auto health = healthMgr.getComponent(e1))
                {
                    float damage = calculateDamage(attack);
                    applyDamage(e1, damage);
                    resetCooldown(e2);
                    AXLOG("%s %u took %f damage from weapon %u. HP left: %f", type1.c_str(), e1, damage, e2,
                          health->currentHealth);
                }
            }
        }
    }
}

//Công thức tính dame
float HealthSystem::calculateDamage(const AttackComponent* attack) const
{
    return (attack->baseDamage + attack->flatBonus) * (1.0f + attack->damageMultiplier);
}

void HealthSystem::applyDamage(Entity target, float damage)
{
    if (auto health = healthMgr.getComponent(target))
    {
        health->currentHealth -= damage;
        AXLOG("%d : %f", target, health->currentHealth);
        if (health->currentHealth <= 0.0f)
        {
            health->currentHealth = 0.0f;
            entityManager.destroyEntity(target);  // Hủy entity nếu hết máu
            AXLOG("Entity %u destroyed due to zero health", target);

            // Gọi callback nếu entity là player hoặc enemy
            if (auto id = identityMgr.getComponent(target))
            {
                
                auto spawnSystem = SystemManager::getInstance()->getSystem<SpawnSystem>();
                if (id->type == "enemy" && spawnSystem->onEnemyDeath)
                {
                    spawnSystem->onEnemyDeath();
                    auto itemSystem = SystemManager::getInstance()->getSystem<ItemSystem>();
                    itemSystem->spawnItemAtDeath(target,false);
                }
                if (id->type == "player" && onPlayerOutOfHealth)
                {
                    onPlayerOutOfHealth();
                }
            }
        }
    }
}

//Kiểm tra cooldown xem có được phép gây sát thương không
bool HealthSystem::canDealDamage(Entity attacker)
{
    if (auto cooldown = cooldownMgr.getComponent(attacker))
    {
        return cooldown->cooldownTimer <= 0.0f;
    }
    return true;  // Nếu không có CooldownComponent, mặc định cho phép gây sát thương
}

//Vào trạng thái cooldown
void HealthSystem::resetCooldown(Entity attacker)
{
    if (auto cooldown = cooldownMgr.getComponent(attacker))
    {
        cooldown->cooldownTimer = cooldown->cooldownDuration;
    }
}

float HealthSystem::getPlayerMaxHealth() const
{
    Entity playerEntity = SystemManager::getInstance()->getSystem<SpawnSystem>()->getPlayerEntity();

    return healthMgr.getComponent(playerEntity)->maxHealth;
}

float HealthSystem::getPlayerCurrentHealth() const
{
    Entity playerEntity = SystemManager::getInstance()->getSystem<SpawnSystem>()->getPlayerEntity();

    return healthMgr.getComponent(playerEntity)->currentHealth;
}
