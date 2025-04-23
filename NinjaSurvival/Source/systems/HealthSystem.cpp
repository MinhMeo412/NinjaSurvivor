#include "Utils.h"
#include "ShopSystem.h"
#include "HealthSystem.h"
#include "SystemManager.h"
#include "CameraSystem.h"
#include "SpawnSystem.h"
#include "MovementSystem.h"
#include "ItemSystem.h"
#include "DamageTextSystem.h"
#include "CollisionSystem.h"
#include "WeaponSystem.h"
#include "CleanupSystem.h"
#include "gameUI/GameOverGamePauseLayer.h"
#include "scenes/GameScene.h"
#include "AudioManager.h"

using namespace ax;

HealthSystem::HealthSystem(EntityManager& em,
                           ComponentManager<IdentityComponent>& im,
                           ComponentManager<HealthComponent>& hm,
                           ComponentManager<AttackComponent>& am,
                           ComponentManager<CooldownComponent>& cm,
                           ComponentManager<DurationComponent>& drm)
    : entityManager(em), identityMgr(im), healthMgr(hm), attackMgr(am), cooldownMgr(cm), durationMgr(drm)
{}

void HealthSystem::init()
{
    attackBuffMultiplier = ShopSystem::getInstance()->getShopBuff("Attack");

    // set callback trong init
    onPlayerOutOfHealth = [this]() {
        //Nhạc game over
        AudioManager::getInstance()->playSound("game_over", false, 1.5f, "music");

        //Gọi UI game over
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
                // gameScene->unscheduleUpdate();          // Dừng update
                SystemManager::getInstance()->setUpdateState(false);
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
    // Kiểm tra identity component và lấy type (nếu có)
    auto id1 = identityMgr.getComponent(e1);
    auto id2 = identityMgr.getComponent(e2);
    if (!id1 || !id2)
        return;
    std::string type1 = id1->type;  // Type1 luôn là player
    std::string type2 = id2->type;

    // Xử lý va chạm giữa enemy/enemy_projectile/boss với player
    if (type1 == "player" && (type2 == "enemy" || type2 == "enemy_projectile" || type2 == "boss"))
    {
        if (auto attack = attackMgr.getComponent(e2))
        {
            if (canDealDamage(e2))
            {
                if (auto health = healthMgr.getComponent(e1))
                {
                    float damage = calculateEnemyDamage(attack);
                    AudioManager::getInstance()->playSound("get_hit", false, 1.0f, "effect");
                    applyDamage(e1, damage);
                    resetCooldown(e2);
                    AXLOG("Player %u took %f damage from %s %u. HP left: %f", e1, damage, type2.c_str(), e2,
                          health->currentHealth);
                }
            }
        }
    }
}

void HealthSystem::handleWeaponCollision(std::unordered_map<Entity, std::vector<Entity>> damagedEnemy)
{
    for (const auto& pair : damagedEnemy)
    {
        std::string type1        = identityMgr.getComponent(pair.first)->type;
        std::vector<Entity> list = pair.second;

        for (const Entity& e : list)
        {
            std::string type2 = identityMgr.getComponent(e)->type;

            if ((type1 == "weapon_melee" || type1 == "weapon_projectile") && (type2 == "enemy" || type2 == "boss"))
            {
                if (auto attack = attackMgr.getComponent(pair.first))
                {
                    if (auto health = healthMgr.getComponent(e))
                    {
                        if (canWeaponDealDame(pair.first, e))
                        {
                            float damage = calculatePlayerDamage(attack);
                            applyDamage(e, damage);

                            SystemManager::getInstance()->getSystem<DamageTextSystem>()->showDamage(damage, e);
                            SystemManager::getInstance()->getSystem<DamageTextSystem>()->showHitEffect(e);
                            AudioManager::getInstance()->playSound("hit", false, 1.5f, "effect");

                            AXLOG("%s %u took %f damage from weapon %u. HP left: %f", type2.c_str(), e, damage,
                                  pair.first, health->currentHealth);

                            // Gây dame xung quanh nếu là explosion_kunai
                            if (identityMgr.getComponent(pair.first)->name == "explosion_kunai")
                            {
                                AudioManager::getInstance()->playSound("explosion_kunai", false, 1.0f, "effect");
                                SystemManager::getInstance()->getSystem<DamageTextSystem>()->showExplosionEffect(e);
                                auto nearbyEnemies = SystemManager::getInstance()->getSystem<CollisionSystem>()->getNearbyEnemyEntities(e);
                                for (auto& enemy : nearbyEnemies)
                                {
                                    applyDamage(enemy, damage);
                                }
                            }

                            if (durationMgr.getComponent(pair.first))
                            {
                                durationMgr.getComponent(pair.first)->timer[e] =
                                    1.0f;  // Thêm vào map tgian chờ gây dame lần tiếp theo là 1s
                            }
                        }
                    }
                }
            }
        }
    }
    // Xóa danh sách damagedEnemy sau khi xử lý để tránh tính sát thương lặp lại
    this->damagedEnemy.clear();
}

// Công thức tính dame
float HealthSystem::calculateEnemyDamage(const AttackComponent* attack)
{
    return (attack->baseDamage) * (1.0f + attack->damageMultiplier) * (1.0 + damageReceiveMultiplier);
}
float HealthSystem::calculatePlayerDamage(const AttackComponent* attack)
{
    return (attack->baseDamage) * (1.0f + attack->damageMultiplier) * (1.0f + attackBuffMultiplier) * Utils::getRandomFloat(1.0, 1.3);
}

void HealthSystem::applyDamage(Entity target, float damage)
{
    if (auto health = healthMgr.getComponent(target))
    {
        health->currentHealth -= damage;
     
        AXLOG("%d : %f", target, health->currentHealth);
        if (health->currentHealth <= 0.0f)
        {
            // Gọi callback nếu entity là player hoặc enemy
            if (auto id = identityMgr.getComponent(target))
            {
                auto spawnSystem = SystemManager::getInstance()->getSystem<SpawnSystem>();
                if (id->type == "enemy" && spawnSystem->onEnemyDeath)
                {
                    spawnSystem->onEnemyDeath();
                    SystemManager::getInstance()->getSystem<ItemSystem>()->spawnItemAtDeath(target, false);
                    // Xóa entity khỏi batch cập nhật di chuyển khi chết
                    SystemManager::getInstance()->getSystem<MovementSystem>()->updateBatchOnDeath(target);

                    // Đếm số kill
                    auto layer = dynamic_cast<GameSceneUILayer*>(SystemManager::getInstance()->getSceneLayer());
                    layer->increaseEnemyKillCount();
                }
                else if (id->type == "boss")
                {
                    SystemManager::getInstance()->getSystem<ItemSystem>()->spawnItemAtDeath(target, true);
                    SystemManager::getInstance()->getSystem<SpawnSystem>()->isBossAlive(false);

                    // Đếm số kill
                    auto layer = dynamic_cast<GameSceneUILayer*>(SystemManager::getInstance()->getSceneLayer());
                    layer->increaseEnemyKillCount();
                }
                else if (id->type == "player" && onPlayerOutOfHealth)
                {
                    onPlayerOutOfHealth();
                }
            }
            SystemManager::getInstance()->getSystem<CleanupSystem>()->destroyEntity(target);  // Hủy entity nếu hết máu
            // AXLOG("Entity %u destroyed due to zero health", target);
        }
    }
}

void HealthSystem::applyBombDamageToAll(float damage)
{
    for (auto entity : entityManager.getActiveEntities())
    {
        auto identity = identityMgr.getComponent(entity);
        if (Utils::in(identity->type, "enemy", "boss"))
        {
            if (auto health = healthMgr.getComponent(entity))
            {
                health->currentHealth -= damage;
                if (health->currentHealth <= 0.0f)
                {
                    auto spawnSystem = SystemManager::getInstance()->getSystem<SpawnSystem>();
                    if (identity->type == "enemy" && spawnSystem->onEnemyDeath)
                    {
                        spawnSystem->onEnemyDeath();
                        SystemManager::getInstance()->getSystem<ItemSystem>()->spawnItemAtDeath(entity, false);
                        // Xóa entity khỏi batch cập nhật di chuyển khi chết
                        SystemManager::getInstance()->getSystem<MovementSystem>()->updateBatchOnDeath(entity);

                        // Đếm số kill
                        auto layer = dynamic_cast<GameSceneUILayer*>(SystemManager::getInstance()->getSceneLayer());
                        layer->increaseEnemyKillCount();
                    }
                    else if (identity->type == "boss")
                    {
                        SystemManager::getInstance()->getSystem<ItemSystem>()->spawnItemAtDeath(entity, true);
                        SystemManager::getInstance()->getSystem<SpawnSystem>()->isBossAlive(false);

                        // Đếm số kill
                        auto layer = dynamic_cast<GameSceneUILayer*>(SystemManager::getInstance()->getSceneLayer());
                        layer->increaseEnemyKillCount();
                    }
                    SystemManager::getInstance()->getSystem<CleanupSystem>()->destroyEntity(
                        entity);  // Hủy entity nếu hết máu
                }
            }
        }
    }
}

// Kiểm tra cooldown xem có được phép gây sát thương không
bool HealthSystem::canDealDamage(Entity attacker)
{
    if (auto cooldown = cooldownMgr.getComponent(attacker))
    {
        return cooldown->cooldownTimer <= 0.0f;
    }
    return true;  // Nếu không có CooldownComponent, mặc định cho phép gây sát thương
}

bool HealthSystem::canWeaponDealDame(Entity weapon, Entity target)
{
    if (auto duration = durationMgr.getComponent(weapon))
    {
        return duration->timer.find(target) == duration->timer.end();
    }
    return true;  // Nếu không có DurationComponent, mặc định cho phép gây sát thương
}

// Vào trạng thái cooldown
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

void HealthSystem::setPlayerCurrentHealth(float hpRecover)
{
    Entity playerEntity = SystemManager::getInstance()->getSystem<SpawnSystem>()->getPlayerEntity();

    healthMgr.getComponent(playerEntity)->currentHealth += hpRecover;
    if (healthMgr.getComponent(playerEntity)->currentHealth > healthMgr.getComponent(playerEntity)->maxHealth)
    {
        healthMgr.getComponent(playerEntity)->currentHealth = healthMgr.getComponent(playerEntity)->maxHealth;
    }

    return;
}
