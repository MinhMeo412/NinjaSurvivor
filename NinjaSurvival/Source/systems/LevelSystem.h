#ifndef __LEVEL_SYSTEM_H__
#define __LEVEL_SYSTEM_H__

#include "System.h"
#include "axmol.h"
#include "components/ComponentManager.h"
#include "entities/EntityManager.h"

class LevelSystem : public System
{
public:
    //LevelSystem();
    LevelSystem(EntityManager& em, ComponentManager<WeaponInventoryComponent>& wim);
    void init() override;
    void update(float dt) override;

    void increaseXP(float xp);
    float getCurrentXP() const { return currentXP; }
    float getNeededXP() const { return neededXP; }
private:
    float currentXP;
    float neededXP;
    int level;
    EntityManager& entityMgr;
    ComponentManager<WeaponInventoryComponent>& wiMgr;

    std::vector<std::string> weapons = {"sword", "shuriken", "kunai"};
    std::vector<std::string> buffs   = {"attack", "health", "speed"};

    void levelUp();
    void chooseWeapon();
    std::vector<std::string> upgradeGenerator();
};


#endif  // !__LEVEL_SYSTEM_H__
