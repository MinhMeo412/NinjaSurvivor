#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "UnitBase.h"

class Character : public UnitBase
{
public:
    Character(UnitType type) : UnitBase(type) {}
}

#endif // __CHARACTER_H__