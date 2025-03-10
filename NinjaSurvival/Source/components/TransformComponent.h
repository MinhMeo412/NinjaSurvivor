#ifndef __TRANSFORM_COMPONENT_H__
#define __TRANSFORM_COMPONENT_H__

#include "axmol.h"

struct TransformComponent
{
    float x     = 0.0f;
    float y     = 0.0f;
    float scale = 1.0f;

    TransformComponent() = default;
    TransformComponent(float xVal, float yVal, float scaleVal = 1.0f) : x(xVal), y(yVal), scale(scaleVal) {}
};

#endif  // __TRANSFORM_COMPONENT_H__
