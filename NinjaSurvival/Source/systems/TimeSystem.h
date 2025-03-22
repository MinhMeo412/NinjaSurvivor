#ifndef __TIME_SYSTEM_H__
#define __TIME_SYSTEM_H__

#include "System.h"
#include "axmol.h"

class TimeSystem : public System
{
public:
    TimeSystem();
    void init() override;
    void update(float dt) override;

    float getElapsedTime() const { return elapsedTime; }

private:
    ax::Layer* uiLayer = nullptr;
    ax::Label* timerLabel = nullptr;
    float elapsedTime = 0.0f;
};


#endif  // !__TIME_SYSTEM_H__
