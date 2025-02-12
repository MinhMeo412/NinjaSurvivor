#include "UnitBase.h"

class Character : public UnitBase
{
public:
    Character(UnitType type) : UnitBase(type) {}

    void update(float dt) override {}
};