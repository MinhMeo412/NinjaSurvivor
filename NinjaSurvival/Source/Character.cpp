#include "Character.h"

using namespace ax;


// on "init" you need to initialize your instance
Character* Character::create(const std::string& spriteSheetPlist, const std::string& spriteFrameName)
{
    Character* character = new (std::nothrow) Character();
    if (character && character->init(spriteSheetPlist, spriteFrameName))
    {
        character->autorelease();
        return character;
    }
    return nullptr;
}

bool Character::init(const std::string& spriteSheetPlist, const std::string& spriteFrameName)
{
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(spriteSheetPlist);
    if (!Sprite::initWithSpriteFrameName(spriteFrameName))
    {
        return false;
    }

    createAnimations();

    _keyboardListener                = EventListenerKeyboard::create();
    _keyboardListener->onKeyPressed  = AX_CALLBACK_2(Character::moveCharacter, this);
    _keyboardListener->onKeyReleased = AX_CALLBACK_2(Character::stopCharacter, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_keyboardListener, this);

    return true;
}

void Character::runAnimation(int tag, const std::string& animationName)
{
    if (_currentAction == nullptr || _currentAction->getTag() != tag)
    {
        if (_currentAction != nullptr)
        {
            stopAction(_currentAction);  // Dừng hành động trước đó nếu có
        }
        // Tạo và chạy animation di chuyển
        _currentAction = RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation(animationName)));
        _currentAction->setTag(tag); 
        runAction(_currentAction);
    }
} 

void Character::update(float delta)
{
    bool isMoving = false;
    Vec2 movement(0, 0);  // Mặc định không di chuyển
    float adjustedSpeed = _speed / sqrt(2.0f);

    if (keyStates[ax::EventKeyboard::KeyCode::KEY_LEFT_ARROW] &&
        keyStates[ax::EventKeyboard::KeyCode::KEY_DOWN_ARROW])
    {
        movement.x = -adjustedSpeed * delta;
        movement.y = -adjustedSpeed * delta;
        runAnimation(ACTION_TAG_LEFT, "left");
        isMoving       = true;
        //AXLOG("Move left down");
    }
    else if (keyStates[ax::EventKeyboard::KeyCode::KEY_LEFT_ARROW] &&
             keyStates[ax::EventKeyboard::KeyCode::KEY_UP_ARROW])
    {
        movement.x = -adjustedSpeed * delta;
        movement.y = adjustedSpeed * delta;
        runAnimation(ACTION_TAG_LEFT, "left");
        isMoving       = true;
        //AXLOG("Move left up");
    }
    else if (keyStates[ax::EventKeyboard::KeyCode::KEY_RIGHT_ARROW] &&
             keyStates[ax::EventKeyboard::KeyCode::KEY_DOWN_ARROW])
    {
        movement.x = adjustedSpeed * delta;
        movement.y = -adjustedSpeed * delta;
        runAnimation(ACTION_TAG_RIGHT, "right");
        isMoving   = true;
        //AXLOG("Move right down");
    }
    else if (keyStates[ax::EventKeyboard::KeyCode::KEY_RIGHT_ARROW] &&
             keyStates[ax::EventKeyboard::KeyCode::KEY_UP_ARROW])
    {
        movement.x = adjustedSpeed * delta;
        movement.y = adjustedSpeed * delta;
        runAnimation(ACTION_TAG_RIGHT, "right");
        isMoving   = true;
        //AXLOG("Move right up");
    }
    else if (keyStates[ax::EventKeyboard::KeyCode::KEY_LEFT_ARROW])
    {
        movement.x = -_speed * delta;
        runAnimation(ACTION_TAG_LEFT, "left");
        isMoving   = true;
        //AXLOG("Move left");
    }
    else if (keyStates[ax::EventKeyboard::KeyCode::KEY_RIGHT_ARROW])
    {
        movement.x = _speed * delta;
        runAnimation(ACTION_TAG_RIGHT, "right");
        isMoving   = true;
        //AXLOG("Move right");
    }
    else if (keyStates[ax::EventKeyboard::KeyCode::KEY_UP_ARROW])
    {
        movement.y = _speed * delta;
        runAnimation(ACTION_TAG_UP, "up");
        isMoving   = true;
        //AXLOG("Move up");
    }
    else if (keyStates[ax::EventKeyboard::KeyCode::KEY_DOWN_ARROW])
    {
        movement.y = -_speed * delta;
        runAnimation(ACTION_TAG_DOWN, "down");
        isMoving   = true;
        //AXLOG("Move down");
    }

    //// Cập nhật vị trí nhân vật
    if (movement != Vec2::ZERO)
    {
        setPosition(getPosition() + movement);
    }
    else if (!isMoving)  // Nếu không di chuyển và không có animation đang chạy
    {
        stopAllActions();  // Dừng tất cả các hành động khi không di chuyển
        // Nếu bạn muốn có animation idle, bạn có thể chạy một animation idle ở đây.
    }
}






void Character::setSpeed(float speed)
{
    _speed = speed;
}

float Character::getSpeed() const
{
    return _speed;
}

void Character::createAnimations()
{
    _downAnimation  = createAnimation("./character_down%d", 3, 0.2f);
    _upAnimation    = createAnimation("./character_up%d", 3, 0.2f);
    _leftAnimation  = createAnimation("./character_left%d", 3, 0.2f);
    _rightAnimation = createAnimation("./character_right%d", 3, 0.2f);

    AnimationCache::getInstance()->addAnimation(_downAnimation, "down");
    AnimationCache::getInstance()->addAnimation(_upAnimation, "up");
    AnimationCache::getInstance()->addAnimation(_leftAnimation, "left");
    AnimationCache::getInstance()->addAnimation(_rightAnimation, "right");
}

Animation* Character::createAnimation(const std::string& format, int frameCount, float delay)
{
    Vector<SpriteFrame*> frames;
    for (int i = 0; i < frameCount; ++i)
    {
        std::string frameName = StringUtils::format(format.c_str(), i);
        SpriteFrame* frame    = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
        if (frame)
        {
            frames.pushBack(frame);
        }
        else
        {
            AXLOG("Frame not found: %s", frameName.c_str());
        }
    }

    if (frames.empty())
    {
        AXLOG("No frames found for animation: %s", format.c_str());
        return nullptr;  // Trả về nullptr nếu không có frame nào
    }

    return Animation::createWithSpriteFrames(frames, delay);
}

void Character::moveCharacter(EventKeyboard::KeyCode keyCode, Event* event)
{
    keyStates[keyCode] = true;
}

void Character::stopCharacter(EventKeyboard::KeyCode keyCode, Event* event)
{
    keyStates[keyCode] = false;
}
