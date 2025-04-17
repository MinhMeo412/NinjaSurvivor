#ifndef __ATTACK_COMPONENT_H__
#define __ATTACK_COMPONENT_H__

struct AttackComponent
{
    float baseDamage;
    float damageMultiplier;

    AttackComponent(float base = 0.0f, float multiplier = 0.0f)
        : baseDamage(base), damageMultiplier(multiplier)
    {}
};

#endif  // __ATTACK_COMPONENT_H__
