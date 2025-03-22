#ifndef __ATTACK_COMPONENT_H__
#define __ATTACK_COMPONENT_H__

struct AttackComponent
{
    float baseDamage;
    float flatBonus;
    float damageMultiplier;

    AttackComponent(float base = 0.0f, float bonus = 0.0f, float multiplier = 0.0f)
        : baseDamage(base), flatBonus(bonus), damageMultiplier(multiplier)
    {}
};

#endif  // __ATTACK_COMPONENT_H__
