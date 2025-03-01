#ifndef __VELOCITY_COMPONENT_H__
#define __VELOCITY_COMPONENT_H__

struct VelocityComponent
{
    float x = 0.0f;
    float y = 0.0f;

    VelocityComponent() = default;
    VelocityComponent(float xVal, float yVal) : x(xVal), y(yVal) {}
};

#endif  // __VELOCITY_COMPONENT_H__
