#ifndef __MAP_SYSTEM_H__
#define __MAP_SYSTEM_H__

#include "System.h"
#include "axmol.h"

class MapSystem : public System
{
public:
    MapSystem();
    void setMapFile(const std::string& tmxFile);
    void setScene(ax::Scene* scene);            
    void init() override;                   
    void update(float dt) override;          
    ax::TMXTiledMap* getTiledMap() const;      

private:
    ax::Scene* parentScene = nullptr;   
    std::string mapFile;            
    ax::TMXTiledMap* tiledMap = nullptr;  
};



#endif  // __MAP_SYSTEM_H__
