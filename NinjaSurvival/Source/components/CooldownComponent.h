#ifndef __COOLDOWN_COMPONENT_H__
#define __COOLDOWN_COMPONENT_H__

struct CooldownComponent
{ //Hiện dùng cho cooldown tgian gây dame
    float cooldownDuration;  // Thời gian cooldown
    float cooldownTimer;     // Thời gian còn lại để hết cooldown

    CooldownComponent(float duration = 1.0f) : cooldownDuration(duration), cooldownTimer(0.0f) {}
};

#endif  // __COOLDOWN_COMPONENT_H__
