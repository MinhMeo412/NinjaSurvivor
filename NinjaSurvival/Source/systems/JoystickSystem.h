#ifndef __JOYSTICK_SYSTEM_H__
#define __JOYSTICK_SYSTEM_H__

#include "System.h"
#include "components/Components.h"
#include "axmol.h"

class JoystickSystem : public System
{
public:
    JoystickSystem();
    void init() override;
    void update(float dt) override;  

    ax::Vec2 getDirection() const;
private:
    ax::Layer* parentLayer     = nullptr;
    ax::Sprite* joystickBase   = nullptr;
    ax::Sprite* joystickHandle = nullptr;
    ax::Vec2 direction; //Hướng di chuyển trả về khi sử dụng joystick
    float maxDistance = 0.0f;  // Khoảng cách tối đa handle có thể di chuyển
    bool isDragging   = false;
    bool isVisible    = false; //joystick ẩn
};



#endif  // __JOYSTICK_SYSTEM_H__
