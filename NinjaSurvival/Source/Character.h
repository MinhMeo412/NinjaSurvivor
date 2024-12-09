#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "axmol.h"


class Character : public ax::Sprite
{
public:

    static Character* create(const std::string& spriteSheetPlist, const std::string& spriteFrameName);

    virtual bool init(const std::string& spriteSheetPlist, const std::string& spriteFrameName);
    virtual void update(float delta);

    void setSpeed(float speed);
    float getSpeed() const;

private:
    void createAnimations();
    void runAnimation(int tag, const std::string& animationName);
    void moveCharacter(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    void stopCharacter(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);

    ax::Animation* createAnimation(const std::string& format, int frameCount, float delay);

    float _speed = 150.0f;
    ax::Vec2 _velocity;
    ax::Action* _currentAction = nullptr;
    static const int ACTION_TAG_LEFT  = 1;
    static const int ACTION_TAG_RIGHT = 2;
    static const int ACTION_TAG_UP    = 3;
    static const int ACTION_TAG_DOWN  = 4;

    ax::EventListenerKeyboard* _keyboardListener = nullptr;

    ax::Animation* _downAnimation       = nullptr;
    ax::Animation* _upAnimation         = nullptr;
    ax::Animation* _leftAnimation       = nullptr;
    ax::Animation* _rightAnimation      = nullptr;

    std::unordered_map<ax::EventKeyboard::KeyCode, bool> keyStates;

};

#endif  // __CHARACTER_H__
