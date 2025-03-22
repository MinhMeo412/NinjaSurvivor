#ifndef __HEALTH_COMPONENT_H__
#define __HEALTH_COMPONENT_H__

struct HealthComponent
{
    float maxHealth;
    float currentHealth;

    HealthComponent(float max = 0.0f) : maxHealth(max), currentHealth(max) {}
};

#endif  // __HEALTH_COMPONENT_H__
