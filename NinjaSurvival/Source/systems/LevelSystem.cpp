#include "LevelSystem.h"
#include "SystemManager.h"

LevelSystem::LevelSystem() {}

void LevelSystem::init()
{
    currentXP = 0;
    neededXP  = 10;
    level     = 1;
}

void LevelSystem::update(float dt)
{
    if (currentXP >= neededXP)
    {
        levelUp();

        chooseWeapon();
    }
}

void LevelSystem::levelUp()
{
    level     = level + 1;
    currentXP = currentXP - neededXP;
    neededXP  = 5 * (level + 1);

    // Tính neededXP với hệ số tăng trưởng dựa trên mốc cấp độ
    float growthMultiplier = 1.0f;  // Hệ số mặc định
    if (level >= 30)
    {
        growthMultiplier = 2.0f;  // Tăng gấp đôi độ khó từ cấp 30
    }
    else if (level >= 20)
    {
        growthMultiplier = 1.5f;  // Tăng 50% độ khó từ cấp 20
    }
    else if (level >= 10)
    {
        growthMultiplier = 1.2f;  // Tăng 20% độ khó từ cấp 10
    }

    neededXP = 5 * (level + 1) * growthMultiplier;
    AXLOG("Level: %d", level);
}

void LevelSystem::increaseXP(float xp)
{
    currentXP = currentXP + xp; //thêm buff

    //AXLOG("Xp hiện tại: + %f = %f", xp, currentXP);
}

void LevelSystem::chooseWeapon()
{

}

