#include "DamageTextSystem.h"
#include "SystemManager.h"
#include "RenderSystem.h"

DamageTextSystem::DamageTextSystem(EntityManager& em,
                                   ComponentManager<TransformComponent>& tm)
    : entityManager(em), transformMgr(tm)
{}
void DamageTextSystem::init()
{
    // Load plist vào SpriteFrameCache
    ax::SpriteFrameCache::getInstance()->addSpriteFramesWithFile("number.plist"); //Có thể load vào loading scene cho gọn
}

void DamageTextSystem::update(float dt)
{
    // Update damage texts
    std::vector<size_t> textsToRemove;
    for (size_t i = 0; i < damageTexts.size(); ++i)
    {
        updateDamageText(damageTexts[i], dt);

        // Xóa nếu hết thời gian sống
        if (damageTexts[i].lifetime <= 0.0f)
        {
            for (auto sprite : damageTexts[i].digits)
            {
                sprite->removeFromParent();
            }
            textsToRemove.push_back(i);
        }
    }
    // Xóa các DamageText đã hoàn thành
    for (auto it = textsToRemove.rbegin(); it != textsToRemove.rend(); ++it)
    {
        damageTexts.erase(damageTexts.begin() + *it);
    }

    // Update hit effects
    std::vector<size_t> effectsToRemove;
    for (size_t i = 0; i < hitEffects.size(); ++i)
    {
        updateHitEffect(hitEffects[i], dt);
        if (hitEffects[i].lifetime <= 0.0f)
        {
            hitEffects[i].sprite->removeFromParent();
            effectsToRemove.push_back(i);
        }
    }
    for (auto it = effectsToRemove.rbegin(); it != effectsToRemove.rend(); ++it)
    {
        hitEffects.erase(hitEffects.begin() + *it);
    }

    // Update explosion effects
    std::vector<size_t> explosionToRemove;
    for (size_t i = 0; i < explosionEffects.size(); ++i)
    {
        updateExplosionEffect(explosionEffects[i], dt);
        if (explosionEffects[i].lifetime <= 0.0f)
        {
            explosionEffects[i].sprite->removeFromParent();
            explosionToRemove.push_back(i);
        }
    }
    for (auto it = explosionToRemove.rbegin(); it != explosionToRemove.rend(); ++it)
    {
        explosionEffects.erase(explosionEffects.begin() + *it);
    }
}

void DamageTextSystem::showDamage(float damage, Entity entity)
{
    createDamageText(damage, entity);
}

void DamageTextSystem::showHitEffect(Entity entity)
{
    createHitEffect(entity);
}

void DamageTextSystem::showExplosionEffect(Entity entity)
{
    createExplosionEffect(entity);
}

void DamageTextSystem::createDamageText(float damage, Entity entity)
{
    auto renderSystem     = SystemManager::getInstance()->getSystem<RenderSystem>();
    auto transform        = transformMgr.getComponent(entity);

    int damageInt         = static_cast<int>(damage);
    std::string damageStr = std::to_string(damageInt);

    float totalWidth  = damageStr.length() * 7.0f;  // Mỗi chữ số rộng 7 pixel
    ax::Vec2 startPos = ax::Vec2(transform->x, transform->y) - ax::Vec2(totalWidth / 2.0f, 0);

    DamageText text;
    text.position = startPos;
    text.lifetime = 1.5f;  // Tổng thời gian hiển thị (1s trôi lên + 0.5s fade out)

    for (size_t i = 0; i < damageStr.length(); ++i)
    {
        int digit             = damageStr[i] - '0';
        std::string frameName = "./n" + std::to_string(digit);
        auto spriteFrame      = ax::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (!spriteFrame)
        {
            AXLOG("Error: Sprite frame %s not found", frameName.c_str());
            continue;
        }

        auto sprite = ax::Sprite::createWithSpriteFrame(spriteFrame);
        if (!sprite)
        {
            AXLOG("Error: Failed to create sprite for digit %d", digit);
            continue;
        }

        sprite->setPosition(startPos + ax::Vec2(i * 7.0f, 0));
        renderSystem->setSpriteBatchNodeForSprite(sprite, "number");
        text.digits.push_back(sprite);

        // Hiệu ứng trôi lên và mờ dần
        sprite->runAction(
            ax::Sequence::create(ax::MoveBy::create(1.0f, ax::Vec2(0, 15)), ax::FadeOut::create(0.5f), nullptr));
    }

    damageTexts.push_back(text);
}

void DamageTextSystem::createHitEffect(Entity entity)
{
    auto renderSystem = SystemManager::getInstance()->getSystem<RenderSystem>();
    auto transform    = transformMgr.getComponent(entity);

    std::string frameName = "./hit";
    auto spriteFrame      = ax::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
    if (!spriteFrame)
    {
        AXLOG("Error: Sprite frame %s not found", frameName.c_str());
        return;
    }

    auto sprite = ax::Sprite::createWithSpriteFrame(spriteFrame);
    if (!sprite)
    {
        AXLOG("Error: Failed to create sprite for hit effect");
        return;
    }

    HitEffect effect;
    effect.sprite   = sprite;
    effect.lifetime = 0.5f;  // Thời gian hiển thị hit effect

    sprite->setPosition(ax::Vec2(transform->x, transform->y));
    sprite->setScale(1.0f);  // Bắt đầu từ scale bình thường
    renderSystem->setSpriteBatchNodeForSprite(sprite, "hit_effect");

    // Hiệu ứng scale up 2x và fade out
    sprite->runAction(ax::Sequence::create(ax::ScaleTo::create(0.4f, 3.0f), ax::FadeOut::create(0.1f), nullptr));

    hitEffects.push_back(effect);
}

void DamageTextSystem::createExplosionEffect(Entity entity)
{
    auto renderSystem = SystemManager::getInstance()->getSystem<RenderSystem>();
    auto transform    = transformMgr.getComponent(entity);

    // Tạo animation từ các frame explosion
    auto animation     = ax::Animation::create();
    for (int i = 1; i <= 7; ++i)
    {
        std::string frameName = "./explosion" + std::to_string(i);
        auto spriteFrame      = ax::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (!spriteFrame)
        {
            AXLOG("Error: Sprite frame %s not found", frameName.c_str());
            continue;
        }
        animation->addSpriteFrame(spriteFrame);;
    }


    animation->setDelayPerUnit(0.125f);  // Mỗi frame cách nhau 0.125s
    animation->setLoops(1);            // Chỉ chạy 1 lần

    // Tạo sprite với frame đầu tiên
    std::string initialFrameName = "./explosion1";
    auto initialSpriteFrame      = ax::SpriteFrameCache::getInstance()->getSpriteFrameByName(initialFrameName);
    if (!initialSpriteFrame)
    {
        AXLOG("Error: Initial sprite frame %s not found", initialFrameName.c_str());
        return;
    }

    auto sprite = ax::Sprite::createWithSpriteFrame(initialSpriteFrame);
    if (!sprite)
    {
        AXLOG("Error: Failed to create sprite for explosion effect");
        return;
    }

    ExplosionEffect effect;
    effect.sprite   = sprite;
    effect.lifetime = 1.0f;  // 7 frames * 0.1s = 0.7s

    sprite->setPosition(ax::Vec2(transform->x, transform->y));
    renderSystem->setSpriteBatchNodeForSprite(sprite, "explosion_effect");

    // Chạy animation và fade out ở cuối
    auto animate = ax::Animate::create(animation);
    sprite->runAction(ax::Sequence::create(animate, ax::FadeOut::create(0.1f), nullptr));

    explosionEffects.push_back(effect);
}

void DamageTextSystem::updateDamageText(DamageText& text, float dt)
{
    text.lifetime -= dt;
    if (text.lifetime < 0.0f)
        text.lifetime = 0.0f;
}

void DamageTextSystem::updateHitEffect(HitEffect& effect, float dt)
{
    effect.lifetime -= dt;
    if (effect.lifetime < 0.0f)
        effect.lifetime = 0.0f;
}

void DamageTextSystem::updateExplosionEffect(ExplosionEffect& effect, float dt)
{
    effect.lifetime -= dt;
    if (effect.lifetime < 0.0f)
        effect.lifetime = 0.0f;
}
