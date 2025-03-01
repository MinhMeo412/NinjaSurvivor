#include "MapSystem.h"

using namespace ax;

MapSystem::MapSystem() {}

void MapSystem::setMapFile(const std::string& tmxFile)
{
    mapFile = tmxFile;
}

void MapSystem::setScene(ax::Scene* scene)
{
    parentScene = scene;
}

void MapSystem::init()
{
    if (!mapFile.empty() && parentScene)
    {
        tiledMap = TMXTiledMap::create(mapFile);
        if (tiledMap)
        {
            parentScene->addChild(tiledMap,1);
            tiledMap->setPosition(Vec2(100,100));
            AXLOG("MapSystem: Loaded map %s", mapFile.c_str());
        }
        else
        {
            AXLOG("MapSystem: Failed to load map %s", mapFile.c_str());
        }
    }
}

void MapSystem::update(float dt) {}

TMXTiledMap* MapSystem::getTiledMap() const
{
    return tiledMap;
}
