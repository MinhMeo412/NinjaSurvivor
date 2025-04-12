#ifndef __DURATION_COMPONENT_H__
#define __DURATION_COMPONENT_H__

struct DurationComponent
{
    float duration = 0.0f;
    std::unordered_map <unsigned int, float> timer;

    DurationComponent() = default;
    DurationComponent(float duration) : duration(duration) {}
};

#endif  // __DURATION_COMPONENT_H__
