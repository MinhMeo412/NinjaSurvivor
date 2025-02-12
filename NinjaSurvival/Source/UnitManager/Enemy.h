#include "UnitBase.h"

class Enemy : public UnitBase
{
public:
    Enemy(UnitType type) : UnitBase(type) {}

    void update(float dt) override {}
}