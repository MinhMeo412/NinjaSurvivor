#ifndef __LEVEL_SYSTEM_H__
#define __LEVEL_SYSTEM_H__

#include "System.h"
#include "axmol.h"

class LevelSystem : public System
{
public:
    LevelSystem();
    void init() override;
    void update(float dt) override;

    void increaseXP(float xp);
    float getCurrentXP() const { return currentXP; }
    float getNeededXP() const { return neededXP; }
private:
    float currentXP;
    float neededXP;
    int level;

    void levelUp();
    void chooseWeapon();
};


#endif  // !__LEVEL_SYSTEM_H__
