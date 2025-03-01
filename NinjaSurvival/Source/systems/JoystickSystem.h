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

    VelocityComponent getVelocity() const;
private:
    ax::Scene* parentScene     = nullptr;
    ax::Sprite* joystickBase   = nullptr;
    ax::Sprite* joystickHandle = nullptr;
    VelocityComponent velocity;
    float maxDistance = 100.0f;             // Khoảng cách tối đa handle có thể di chuyển
    bool isDragging   = false;        
};



#endif  // __JOYSTICK_SYSTEM_H__
