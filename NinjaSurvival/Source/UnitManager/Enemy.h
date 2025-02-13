#ifndef __ENEMY_H__
#define __ENEMY_H__

#include "UnitBase.h"

class Enemy : public UnitBase
{
public:
    Enemy(UnitType type) : UnitBase(type) {}

    void update(float dt) override {}
}

#endif // __ENEMY_H__