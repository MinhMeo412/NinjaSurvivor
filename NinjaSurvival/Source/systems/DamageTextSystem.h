#ifndef __DAMAGE_TEXT_SYSTEM_H__
#define __DAMAGE_TEXT_SYSTEM_H__

#include "System.h"
#include "components/ComponentManager.h"
#include "entities/EntityManager.h"
#include "axmol.h"

class DamageTextSystem : public System
{
public:
    DamageTextSystem(EntityManager& em, ComponentManager<TransformComponent>& tm);

    void init() override;
    void update(float dt) override;
    void showDamage(float damage, Entity entity);

private:
    EntityManager& entityManager;
    ComponentManager<TransformComponent>& transformMgr;

    struct DamageText
    {
        std::vector<ax::Sprite*> digits;  // Danh sách sprite cho các chữ số
        ax::Vec2 position;                // Vị trí hiện tại
        float lifetime;                   // Thời gian sống còn lại
    };
    std::vector<DamageText> damageTexts;  // Danh sách các số sát thương đang hiển thị

    void createDamageText(float damage, Entity entity);
    void updateDamageText(DamageText& text, float dt);
};

#endif  // __DAMAGE_TEXT_SYSTEM_H__
