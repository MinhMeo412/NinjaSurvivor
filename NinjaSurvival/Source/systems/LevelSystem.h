#ifndef __LEVEL_SYSTEM_H__
#define __LEVEL_SYSTEM_H__

#include "System.h"
#include "axmol.h"
#include "components/ComponentManager.h"
#include "entities/EntityManager.h"

class LevelSystem : public System
{
public:
    LevelSystem(EntityManager& em, ComponentManager<WeaponInventoryComponent>& wim);
    void init() override;
    void update(float dt) override;


    void increaseXP(float xp);
    float getCurrentXP() const { return currentXP; }
    float getNeededXP() const { return neededXP; }
    int getLevel() const { return level; }

    int getRerollCount() { return rerollCount; }
    int setRerollCount(int rerollCountLeft) { return rerollCount = rerollCountLeft; }

    std::vector<std::string> upgradeGenerator();

private:
    float currentXP;
    float neededXP;
    int level;
    int rerollCount = 0;
    EntityManager& entityMgr;
    ComponentManager<WeaponInventoryComponent>& wiMgr;

    std::vector<std::string> weapons = {"sword", "shuriken", "kunai"};
    std::vector<std::string> buffs   = {"attack", "health", "speed"};


    void levelUp();
    void chooseWeapon();
};


#endif  // !__LEVEL_SYSTEM_H__
