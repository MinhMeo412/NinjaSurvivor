#include "EntityFactory.h"
#include "GameData.h"

Entity EntityFactory::createEntity(const std::string& type, const std::string& name)
{
    auto gameData = GameData::getInstance();
    const auto& templ = gameData->getEntityTemplates().at(type).at(name);

    Entity entity = entityManager.createEntity();

    if (!templ.type.empty() && !templ.name.empty())
    {
        IdentityComponent identity(templ.type, templ.name);
        identityMgr.addComponent(entity, identity);
    }
    if (templ.transform)
        transformMgr.addComponent(entity, *templ.transform);
    if (templ.sprite)
        spriteMgr.addComponent(entity, *templ.sprite);
    if (templ.animation)
        animationMgr.addComponent(entity, *templ.animation);
    if (templ.velocity)
        velocityMgr.addComponent(entity, *templ.velocity);
    if (templ.hitbox)
        hitboxMgr.addComponent(entity, *templ.hitbox);
    if (templ.health)
        healthMgr.addComponent(entity, *templ.health);
    if (templ.attack)
        attackMgr.addComponent(entity, *templ.attack);
    if (templ.cooldown)
        cooldownMgr.addComponent(entity, *templ.cooldown);
    if (templ.speed)
        speedMgr.addComponent(entity, *templ.speed);

    //thêm if cho các component khác
    return entity;
}
