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
    std::vector<size_t> toRemove;

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
            toRemove.push_back(i);
        }
    }

    // Xóa các DamageText đã hoàn thành
    for (auto it = toRemove.rbegin(); it != toRemove.rend(); ++it)
    {
        damageTexts.erase(damageTexts.begin() + *it);
    }
}

void DamageTextSystem::showDamage(float damage, Entity entity)
{
    createDamageText(damage, entity);
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

void DamageTextSystem::updateDamageText(DamageText& text, float dt)
{
    text.lifetime -= dt;
    if (text.lifetime < 0.0f)
        text.lifetime = 0.0f;
}
