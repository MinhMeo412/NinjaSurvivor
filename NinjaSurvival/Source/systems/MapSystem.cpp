#include "MapSystem.h"
#include "SystemManager.h"
#include "GameData.h"

using namespace ax;

MapSystem::MapSystem() {}

void MapSystem::init()
{
    auto systemManager = SystemManager::getInstance();
    parentScene = systemManager->getCurrentScene();

    if (!parentScene)
    {
        AXLOG("MapSystem: No parent scene set");
        return;
    }

    auto gameData               = GameData::getInstance();
    std::string selectedMapName = gameData->getSelectedMap();
    if (selectedMapName.empty())
    {
        AXLOG("MapSystem: No selected map in GameData");
        return;
    }

    const auto* map = gameData->getMap(selectedMapName);
    if (!map || map->tmxFile.empty())
    {
        AXLOG("MapSystem: Failed to find tmxFile for map %s", selectedMapName.c_str());
        return;
    }

    tiledMap = TMXTiledMap::create(map->tmxFile);
    if (tiledMap)
    {
        parentScene->addChild(tiledMap, 1);
        tiledMap->setPosition(Vec2(100, 100));
        AXLOG("MapSystem: Loaded map %s", map->tmxFile.c_str());
    }
    else
    {
        AXLOG("MapSystem: Failed to load map %s", map->tmxFile.c_str());
    }
}

void MapSystem::update(float dt) {}

//TMXTiledMap* MapSystem::getTiledMap() const
//{
//    return tiledMap;
//}
