#include "EntityFactory.h"
#include "GameData.h"

Entity EntityFactory::createEntity(const std::string& type, const std::string& name)
{
    auto gameData = GameData::getInstance();
    auto templ    = gameData->getEntityTemplates().at(type).at(name);

    Entity entity = entityManager.createEntity();

    if (templ.transform)
        transformMgr.addComponent(entity, *templ.transform);
    if (templ.sprite)
        spriteMgr.addComponent(entity, *templ.sprite);
    if (templ.animation)
        animationMgr.addComponent(entity, *templ.animation);
    if (templ.velocity)
        velocityMgr.addComponent(entity, *templ.velocity);

    //thêm if cho các component khác
    return entity;
}
