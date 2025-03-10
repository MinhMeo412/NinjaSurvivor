#ifndef __MAP_SYSTEM_H__
#define __MAP_SYSTEM_H__

#include "System.h"
#include "axmol.h"

class MapSystem : public System
{
public:
    MapSystem();      
    void init() override;                   
    void update(float dt) override;          
    //ax::TMXTiledMap* getTiledMap() const;      

private:
    ax::Scene* parentScene = nullptr;          
    ax::TMXTiledMap* tiledMap = nullptr;  
};



#endif  // __MAP_SYSTEM_H__
