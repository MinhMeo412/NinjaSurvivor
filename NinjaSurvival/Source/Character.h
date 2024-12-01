#ifndef __CHARACTER_H__
#define __CHARACTER_H__

#include "axmol.h"


class Character : public ax::Sprite
{
public:
    static Character* create(const std::string& spriteFile);
    virtual bool init(const std::string& spriteFile);

    /*static Character* create(const std::string& spriteSheetPlist, const std::string& spriteFrameName);

    virtual bool init(const std::string& spriteSheetPlist, const std::string& spriteFrameName);*/
    virtual void update(float delta);

    void setSpeed(float speed);
    float getSpeed() const;

private:
    /*void createAnimations();
    void moveCharacter(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    void stopCharacter(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);

    ax::Animation* createAnimation(const std::string& format, int frameCount, float delay);

    float _speed = 150.0f;
    ax::Vec2 _velocity;
    ax::Action* _currentAction = nullptr;
    ax::EventListenerKeyboard* _keyboardListener = nullptr;

    ax::Animation* _downAnimation       = nullptr;
    ax::Animation* _upAnimation         = nullptr;
    ax::Animation* _leftAnimation       = nullptr;
    ax::Animation* _rightAnimation      = nullptr;*/


    void moveCharacter(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
    void stopCharacter(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);

    float _speed = 150.0f;
    ax::Vec2 _velocity;
    ax::Action* _currentAction                   = nullptr;
    ax::EventListenerKeyboard* _keyboardListener = nullptr;

    std::unordered_set<ax::EventKeyboard::KeyCode> pressedKeys;
};

#endif  // __CHARACTER_H__
