#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

#include "axmol.h"


class Joystick : public ax::Node
{
public:

    static Joystick* create();

    bool init();

    ax::Vec2 getDirection() const;

private:
    ax::Sprite* _base;
    ax::Sprite* _thumb;
    ax::Vec2 _direction;

    bool _isTouching = false;

    void onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);

};

#endif  // __JOYSTICK_H__
