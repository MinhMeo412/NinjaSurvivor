#ifndef __BOSS_H__
#define __BOSS_H__

#include "UnitBase.h"

class Boss : public UnitBase
{
public:
    Boss(UnitType type) : UnitBase(type) {}
}

#endif // __BOSS_H__