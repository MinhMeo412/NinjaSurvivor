#ifndef __HITBOX_COMPONENT_H__
#define __HITBOX_COMPONENT_H__

#include "axmol.h"

struct HitboxComponent
{
    ax::Size size = ax::Size(0,0);
    ax::Size defaultSize = ax::Size(0, 0);

    HitboxComponent() = default;
    HitboxComponent(float width = 0.0f, float height = 0.0f) : size(width, height), defaultSize(width, height) {}
};

#endif  // __HITBOX_COMPONENT_H__
