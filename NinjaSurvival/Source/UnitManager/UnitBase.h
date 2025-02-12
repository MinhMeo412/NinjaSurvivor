#include <unordered_map>
#include "axmol.h"

enum class UnitType
{
    CHARACTER_1,
    CHARACTER_2,
    CHARACTER_3,
    ENEMY_1,
    ENEMY_2,
    ENEMY_3,
    ENEMY_4,
    ENEMY_5,
    ENEMY_6,
    BOSS_1,
    BOSS_2,
    BOSS_3
};

struct UnitStats
{
    int hp;
    int attack;
    float moveSpeed;
    float attackRange;
    float attackRate;
};

std::unordered_map<UnitType, UnitStats> unitData = {
    {UnitType::CHARACTER_1, {100, 15, 180, 50, 1.2f}}, {UnitType::CHARACTER_2, {200, 10, 160, 40, 1.5f}},
    {UnitType::CHARACTER_3, {120, 20, 175, 55, 1.1f}},

    {UnitType::ENEMY_1, {50, 5, 120, -1, -1}},         {UnitType::ENEMY_2, {80, 8, 100, 35, 0.8f}},
    {UnitType::ENEMY_3, {60, 7, 110, 30, 1.0f}},       {UnitType::ENEMY_4, {90, 9, 95, -1, -1}},
    {UnitType::ENEMY_5, {70, 6, 105, 25, 0.9f}},       {UnitType::ENEMY_6, {100, 12, 90, 40, 0.7f}},

    {UnitType::BOSS_1, {500, 50, 80, 100, 0.5f}},      {UnitType::BOSS_2, {700, 60, 90, 120, 0.4f}},
    {UnitType::BOSS_3, {900, 70, 100, 150, 0.3f}}};

class UnitBase : public ax::Node
{
protected:
    int _hp;
    int _attack;
    float _moveSpeed;
    float _attackRange;
    float _attackRate;
    UnitType _type;

public:
    UnitBase(UnitType type) : _type(type) { initData(); }

    void initData()
    {
        if (unitData.find(_type) != unitData.end())
        {
            UnitStats stats = unitData[_type];

            _hp          = stats.hp;
            _attack      = stats.attack;
            _moveSpeed   = stats.moveSpeed;
            _attackRange = stats.attackRange;
            _attackRate  = stats.attackRate;
        }
        else
        {
            AXLOG("ERROR: UnitType not exist!");
        }
    }

    virtual void update(float dt) = 0;

    int getHP() const { return _hp; }
    int getAttack() const { return _attack; }
    float getMoveSpeed() const { return _moveSpeed; }
    float getAttackRange() const { return _attackRange; }
    float getAttackRate() const { return _attackRate; }
};