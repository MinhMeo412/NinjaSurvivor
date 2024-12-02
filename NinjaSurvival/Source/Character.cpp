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

    //SpriteFrameCache::getInstance()->addSpriteFramesWithFile(spriteSheetPlist);
    createAnimations();

    _keyboardListener                = EventListenerKeyboard::create();
    _keyboardListener->onKeyPressed  = AX_CALLBACK_2(Character::moveCharacter, this);
    _keyboardListener->onKeyReleased = AX_CALLBACK_2(Character::stopCharacter, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_keyboardListener, this);

    return true;
}
/*
void Character::update(float delta)
{
    //Vec2 movement(0, 0);  // Mặc định không di chuyển

    //// Kiểm tra các phím đang được nhấn và cập nhật hướng di chuyển
    //if (pressedKeys.count(EventKeyboard::KeyCode::KEY_LEFT_ARROW))
    //{
    //    movement.x = -_speed * delta;
    //    _currentAction = runAction(Animate::create(_leftAnimation));
    //}
    //if (pressedKeys.count(EventKeyboard::KeyCode::KEY_RIGHT_ARROW))
    //{
    //    movement.x = _speed * delta;
    //    _currentAction = runAction(Animate::create(_rightAnimation));
    //}
    //if (pressedKeys.count(EventKeyboard::KeyCode::KEY_UP_ARROW))
    //{
    //    movement.y = _speed * delta;
    //    _currentAction = runAction(Animate::create(_upAnimation));
    //}
    //if (pressedKeys.count(EventKeyboard::KeyCode::KEY_DOWN_ARROW))
    //{
    //    movement.y = -_speed * delta;
    //    _currentAction = runAction(Animate::create(_downAnimation));
    //}

    //// Cập nhật vị trí nhân vật
    //if (movement != Vec2::ZERO)
    //{
    //    setPosition(getPosition() + movement);
    //}

    Vec2 movement(0, 0);  // Mặc định không di chuyển
    bool isMoving = false;

    // Kiểm tra các phím đang được nhấn và cập nhật hướng di chuyển
    if (pressedKeys.count(EventKeyboard::KeyCode::KEY_LEFT_ARROW))
    {
        movement.x = -_speed * delta;
        if (_currentDirection != "left")
        {
            _currentDirection = "left";
            stopAllActions();  // Dừng tất cả các hành động cũ
            runAction(RepeatForever::create(Animate::create(_leftAnimation)));  // Chạy animation liên tục
        }
        isMoving = true;
    }
    else if (pressedKeys.count(EventKeyboard::KeyCode::KEY_RIGHT_ARROW))
    {
        movement.x = _speed * delta;
        if (_currentDirection != "right")
        {
            _currentDirection = "right";
            stopAllActions();  // Dừng tất cả các hành động cũ
            runAction(RepeatForever::create(Animate::create(_rightAnimation)));  // Chạy animation liên tục
        }
        isMoving = true;
    }
    else if (pressedKeys.count(EventKeyboard::KeyCode::KEY_UP_ARROW))
    {
        movement.y = _speed * delta;
        if (_currentDirection != "up")
        {
            _currentDirection = "up";
            stopAllActions();  // Dừng tất cả các hành động cũ
            runAction(RepeatForever::create(Animate::create(_upAnimation)));  // Chạy animation liên tục
        }
        isMoving = true;
    }
    else if (pressedKeys.count(EventKeyboard::KeyCode::KEY_DOWN_ARROW))
    {
        movement.y = -_speed * delta;
        if (_currentDirection != "down")
        {
            _currentDirection = "down";
            stopAllActions();  // Dừng tất cả các hành động cũ
            runAction(RepeatForever::create(Animate::create(_downAnimation)));  // Chạy animation liên tục
        }
        isMoving = true;
    }

    // Cập nhật vị trí nhân vật nếu có chuyển động
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
*/

void Character::update(float delta)
{
    Vec2 movement(0, 0);  // Mặc định không di chuyển
    bool isMoving            = false;
    bool hasDirectionChanged = false;

    // Kiểm tra phím trái
    if (pressedKeys.count(EventKeyboard::KeyCode::KEY_LEFT_ARROW))
    {
        movement.x = -_speed * delta;
        if (_currentDirection != "left")  // Nếu hướng chưa phải là left
        {
            if (_lastDirection == "")  // Nếu chưa có phím nào được nhấn trước
            {
                stopAllActions();  // Dừng tất cả các hành động cũ
                runAction(RepeatForever::create(Animate::create(_leftAnimation)));  // Chạy animation left
                _currentDirection = "left";                                         // Cập nhật hướng
                _lastDirection    = "left";                                         // Lưu lại phím được nhấn
            }
            hasDirectionChanged = true;  // Đánh dấu hướng đã thay đổi
        }
        isMoving = true;
    }

    // Kiểm tra phím phải
    if (pressedKeys.count(EventKeyboard::KeyCode::KEY_RIGHT_ARROW))
    {
        movement.x = _speed * delta;
        if (_currentDirection != "right")  // Nếu hướng chưa phải là right
        {
            if (_lastDirection == "")  // Nếu chưa có phím nào được nhấn trước
            {
                stopAllActions();  // Dừng tất cả các hành động cũ
                runAction(RepeatForever::create(Animate::create(_rightAnimation)));  // Chạy animation right
                _currentDirection = "right";                                         // Cập nhật hướng
                _lastDirection    = "right";                                         // Lưu lại phím được nhấn
            }
            hasDirectionChanged = true;  // Đánh dấu hướng đã thay đổi
        }
        isMoving = true;
    }

    // Kiểm tra phím lên
    if (pressedKeys.count(EventKeyboard::KeyCode::KEY_UP_ARROW))
    {
        movement.y = _speed * delta;
        if (_currentDirection != "up")  // Nếu hướng chưa phải là up
        {
            if (_lastDirection == "")  // Nếu chưa có phím nào được nhấn trước
            {
                stopAllActions();  // Dừng tất cả các hành động cũ
                runAction(RepeatForever::create(Animate::create(_upAnimation)));  // Chạy animation up
                _currentDirection = "up";                                         // Cập nhật hướng
                _lastDirection    = "up";                                         // Lưu lại phím được nhấn
            }
            hasDirectionChanged = true;  // Đánh dấu hướng đã thay đổi
        }
        isMoving = true;
    }

    // Kiểm tra phím xuống
    if (pressedKeys.count(EventKeyboard::KeyCode::KEY_DOWN_ARROW))
    {
        movement.y = -_speed * delta;
        if (_currentDirection != "down")  // Nếu hướng chưa phải là down
        {
            if (_lastDirection == "")  // Nếu chưa có phím nào được nhấn trước
            {
                stopAllActions();  // Dừng tất cả các hành động cũ
                runAction(RepeatForever::create(Animate::create(_downAnimation)));  // Chạy animation down
                _currentDirection = "down";                                         // Cập nhật hướng
                _lastDirection    = "down";                                         // Lưu lại phím được nhấn
            }
            hasDirectionChanged = true;  // Đánh dấu hướng đã thay đổi
        }
        isMoving = true;
    }

    // Xử lý di chuyển chéo: nếu có 2 phím nhấn đồng thời, thì tính toán hướng di chuyển
    if (movement != Vec2::ZERO && !hasDirectionChanged)
    {
        if (pressedKeys.count(EventKeyboard::KeyCode::KEY_LEFT_ARROW) &&
            pressedKeys.count(EventKeyboard::KeyCode::KEY_UP_ARROW))
        {
            movement.x        = -_speed * delta;
            movement.y        = _speed * delta;
            _currentDirection = "left_up";  // Di chuyển chéo lên trái
        }
        else if (pressedKeys.count(EventKeyboard::KeyCode::KEY_LEFT_ARROW) &&
                 pressedKeys.count(EventKeyboard::KeyCode::KEY_DOWN_ARROW))
        {
            movement.x        = -_speed * delta;
            movement.y        = -_speed * delta;
            _currentDirection = "left_down";  // Di chuyển chéo xuống trái
        }
        else if (pressedKeys.count(EventKeyboard::KeyCode::KEY_RIGHT_ARROW) &&
                 pressedKeys.count(EventKeyboard::KeyCode::KEY_UP_ARROW))
        {
            movement.x        = _speed * delta;
            movement.y        = _speed * delta;
            _currentDirection = "right_up";  // Di chuyển chéo lên phải
        }
        else if (pressedKeys.count(EventKeyboard::KeyCode::KEY_RIGHT_ARROW) &&
                 pressedKeys.count(EventKeyboard::KeyCode::KEY_DOWN_ARROW))
        {
            movement.x        = _speed * delta;
            movement.y        = -_speed * delta;
            _currentDirection = "right_down";  // Di chuyển chéo xuống phải
        }
    }

    // Cập nhật vị trí nhân vật nếu có chuyển động
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
    pressedKeys.insert(keyCode);
}

void Character::stopCharacter(EventKeyboard::KeyCode keyCode, Event* event)
{
    pressedKeys.erase(keyCode);
}
