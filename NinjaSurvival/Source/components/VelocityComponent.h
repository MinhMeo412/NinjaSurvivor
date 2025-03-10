#ifndef __VELOCITY_COMPONENT_H__
#define __VELOCITY_COMPONENT_H__

struct VelocityComponent
{
    float vx = 0.0f;
    float vy = 0.0f;

    VelocityComponent() = default;
    VelocityComponent(float vxVal, float vyVal) : vx(vxVal), vy(vyVal) {}
};

#endif  // __VELOCITY_COMPONENT_H__
