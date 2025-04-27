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
    ax::Vec2 direction;  // Hướng di chuyển trả về
    bool isKeyUpPressed    = false;
    bool isKeyDownPressed  = false;
    bool isKeyLeftPressed  = false;
    bool isKeyRightPressed = false;
};

#endif  // __JOYSTICK_SYSTEM_H__
