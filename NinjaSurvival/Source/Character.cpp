#include "Character.h"

using namespace ax;

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

    //To move using the keyboard
    //_keyboardListener                = EventListenerKeyboard::create();
    //_keyboardListener->onKeyPressed  = AX_CALLBACK_2(Character::moveCharacter, this);
    //_keyboardListener->onKeyReleased = AX_CALLBACK_2(Character::stopCharacter, this);
    //_eventDispatcher->addEventListenerWithSceneGraphPriority(_keyboardListener, this);

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




void Character::update(Vec2 direction, float dt)
{
    bool isMoving = false;
    Vec2 movement(0, 0);  // Mặc định không di chuyển
    float adjustedSpeed = _speed / sqrt(2.0f);

    if (direction.x < 0 && direction.y < 0)  // Di chuyển chéo trái xuống
    {
        movement.x = -adjustedSpeed * dt ;
        movement.y = -adjustedSpeed * dt;
        runAnimation(ACTION_TAG_LEFT, "left");
        isMoving = true;
    }
    else if (direction.x < 0 && direction.y > 0)  // Di chuyển chéo trái lên
    {
        movement.x = -adjustedSpeed * dt ;
        movement.y = adjustedSpeed * dt ;
        runAnimation(ACTION_TAG_LEFT, "left");
        isMoving = true;
    }
    else if (direction.x > 0 && direction.y < 0)  // Di chuyển chéo phải xuống
    {
        movement.x = adjustedSpeed * dt ;
        movement.y = -adjustedSpeed * dt ;
        runAnimation(ACTION_TAG_RIGHT, "right");
        isMoving = true;
    }
    else if (direction.x > 0 && direction.y > 0)  // Di chuyển chéo phải lên
    {
        movement.x = adjustedSpeed * dt ;
        movement.y = adjustedSpeed * dt ;
        runAnimation(ACTION_TAG_RIGHT, "right");
        isMoving = true;
    }
    else if (direction.x < 0)  // Di chuyển trái
    {
        movement.x = -_speed * dt ;
        runAnimation(ACTION_TAG_LEFT, "left");
        isMoving = true;
    }
    else if (direction.x > 0)  // Di chuyển phải
    {
        movement.x = _speed * dt ;
        runAnimation(ACTION_TAG_RIGHT, "right");
        isMoving = true;
    }
    else if (direction.y > 0)  // Di chuyển lên
    {
        movement.y = _speed * dt ;
        runAnimation(ACTION_TAG_UP, "up");
        isMoving = true;
    }
    else if (direction.y < 0)  // Di chuyển xuống
    {
        movement.y = -_speed * dt ;
        runAnimation(ACTION_TAG_DOWN, "down");
        isMoving = true;
    }

    if (movement != Vec2::ZERO)
    {
        setPosition(getPosition() + movement);
    }
    else if (!isMoving)  // Nếu không di chuyển và không có animation đang chạy
    {
        stopAllActions();  // Dừng tất cả các hành động khi không di chuyển
        //Có thể chạy một animation idle ở đây.
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


//To move using the keyboard
//void Character::moveCharacter(EventKeyboard::KeyCode keyCode, Event* event)
//{
//    keyStates[keyCode] = true;
//}
//
//void Character::stopCharacter(EventKeyboard::KeyCode keyCode, Event* event)
//{
//    keyStates[keyCode] = false;
//}

// void Character::update(Vec2 direction, float dt)  Using keyboard to move
//{
//     bool isMoving = false;
//     Vec2 movement(0, 0);  // Mặc định không di chuyển
//     float adjustedSpeed = _speed / sqrt(2.0f);
//
//     if (keyStates[ax::EventKeyboard::KeyCode::KEY_LEFT_ARROW] &&
//     keyStates[ax::EventKeyboard::KeyCode::KEY_DOWN_ARROW])
//     {
//         movement.x = -adjustedSpeed * dt;
//         movement.y = -adjustedSpeed * dt;
//         runAnimation(ACTION_TAG_LEFT, "left");
//         isMoving = true;
//     }
//     else if (keyStates[ax::EventKeyboard::KeyCode::KEY_LEFT_ARROW] &&
//              keyStates[ax::EventKeyboard::KeyCode::KEY_UP_ARROW])
//     {
//         movement.x = -adjustedSpeed * dt;
//         movement.y = adjustedSpeed * dt;
//         runAnimation(ACTION_TAG_LEFT, "left");
//         isMoving = true;
//     }
//     else if (keyStates[ax::EventKeyboard::KeyCode::KEY_RIGHT_ARROW] &&
//              keyStates[ax::EventKeyboard::KeyCode::KEY_DOWN_ARROW])
//     {
//         movement.x = adjustedSpeed * dt;
//         movement.y = -adjustedSpeed * dt;
//         runAnimation(ACTION_TAG_RIGHT, "right");
//         isMoving = true;
//     }
//     else if (keyStates[ax::EventKeyboard::KeyCode::KEY_RIGHT_ARROW] &&
//              keyStates[ax::EventKeyboard::KeyCode::KEY_UP_ARROW])
//     {
//         movement.x = adjustedSpeed * dt;
//         movement.y = adjustedSpeed * dt;
//         runAnimation(ACTION_TAG_RIGHT, "right");
//         isMoving = true;
//     }
//     else if (keyStates[ax::EventKeyboard::KeyCode::KEY_LEFT_ARROW])
//     {
//         movement.x = -_speed * dt;
//         runAnimation(ACTION_TAG_LEFT, "left");
//         isMoving = true;
//     }
//     else if (keyStates[ax::EventKeyboard::KeyCode::KEY_RIGHT_ARROW])
//     {
//         movement.x = _speed * dt;
//         runAnimation(ACTION_TAG_RIGHT, "right");
//         isMoving = true;
//     }
//     else if (keyStates[ax::EventKeyboard::KeyCode::KEY_UP_ARROW])
//     {
//         movement.y = _speed * dt;
//         runAnimation(ACTION_TAG_UP, "up");
//         isMoving = true;
//     }
//     else if (keyStates[ax::EventKeyboard::KeyCode::KEY_DOWN_ARROW])
//     {
//         movement.y = -_speed * dt;
//         runAnimation(ACTION_TAG_DOWN, "down");
//         isMoving = true;
//     }
//
//     //// Cập nhật vị trí nhân vật
//     if (movement != Vec2::ZERO)
//     {
//         setPosition(getPosition() + movement);
//     }
//     else if (!isMoving)  // Nếu không di chuyển và không có animation đang chạy
//     {
//         stopAllActions();  // Dừng tất cả các hành động khi không di chuyển
//         // Nếu bạn muốn có animation idle, bạn có thể chạy một animation idle ở đây.
//     }
// }
