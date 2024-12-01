//#include "Character.h"
//
//using namespace ax;
//
//
//// on "init" you need to initialize your instance
//Character* Character::create(const std::string& spriteSheetPlist, const std::string& spriteFrameName)
//{
//    Character* character = new (std::nothrow) Character();
//    if (character && character->init(spriteSheetPlist, spriteFrameName))
//    {
//        character->autorelease();
//        return character;
//    }
//    return nullptr;
//}
//
//bool Character::init(const std::string& spriteSheetPlist, const std::string& spriteFrameName)
//{
//    SpriteFrameCache::getInstance()->addSpriteFramesWithFile(spriteSheetPlist);
//    if (!Sprite::initWithSpriteFrameName(spriteFrameName))
//    {
//        return false;
//    }
//
//    //SpriteFrameCache::getInstance()->addSpriteFramesWithFile(spriteSheetPlist);
//    createAnimations();
//
//    _keyboardListener                = EventListenerKeyboard::create();
//    _keyboardListener->onKeyPressed  = AX_CALLBACK_2(Character::moveCharacter, this);
//    _keyboardListener->onKeyReleased = AX_CALLBACK_2(Character::stopCharacter, this);
//    _eventDispatcher->addEventListenerWithSceneGraphPriority(_keyboardListener, this);
//
//    return true;
//}
//
//void Character::update(float delta)
//{
//    setPosition(getPosition() + _velocity * delta);
//}
//
//void Character::setSpeed(float speed)
//{
//    _speed = speed;
//}
//
//float Character::getSpeed() const
//{
//    return _speed;
//}
//
//void Character::createAnimations()
//{
//    _downAnimation  = createAnimation("./character_down%d", 3, 0.2f);
//    _upAnimation    = createAnimation("./character_up%d", 3, 0.2f);
//    _leftAnimation  = createAnimation("./character_left%d", 3, 0.2f);
//    _rightAnimation = createAnimation("./character_right%d", 3, 0.2f);
//}
//
//Animation* Character::createAnimation(const std::string& format, int frameCount, float delay)
//{
//    Vector<SpriteFrame*> frames;
//    for (int i = 0; i < frameCount; ++i)
//    {
//        std::string frameName = StringUtils::format(format.c_str(), i);
//        SpriteFrame* frame    = SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
//        if (frame)
//            frames.pushBack(frame);
//    }
//    return Animation::createWithSpriteFrames(frames, delay);
//}
//
//void Character::moveCharacter(EventKeyboard::KeyCode keyCode, Event* event)
//{
//
//    stopAllActions();  // Stop any ongoing animation
//    switch (keyCode)
//    {
//    case EventKeyboard::KeyCode::KEY_UP_ARROW:
//        _velocity      = Vec2(0, _speed);
//        _currentAction = runAction(Animate::create(_upAnimation));
//        break;
//    case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
//        _velocity      = Vec2(0, -_speed);
//        _currentAction = runAction(Animate::create(_downAnimation));
//        break;
//    case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
//        _velocity      = Vec2(-_speed, 0);
//        _currentAction = runAction(Animate::create(_leftAnimation));
//        break;
//    case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
//        _velocity      = Vec2(_speed, 0);
//        _currentAction = runAction(Animate::create(_rightAnimation));
//        break;
//    default:
//        break;
//    }
//}
//
//void Character::stopCharacter(EventKeyboard::KeyCode keyCode, Event* event)
//{
//    _velocity = Vec2::ZERO;
//    stopAction(_currentAction);
//    _currentAction = nullptr;
//}


#include "Character.h"

using namespace ax;

// on "init" you need to initialize your instance
Character* Character::create(const std::string& spriteFile)
{
    Character* character = new (std::nothrow) Character();
    if (character && character->init(spriteFile))
    {
        character->autorelease();
        return character;
    }
    return nullptr;
}

bool Character::init(const std::string& spriteFile)
{
    if (!Sprite::initWithFile(spriteFile))
    {
        return false;
    }

    // Set up keyboard listener for movement
    _keyboardListener                = EventListenerKeyboard::create();
    _keyboardListener->onKeyPressed  = AX_CALLBACK_2(Character::moveCharacter, this);
    _keyboardListener->onKeyReleased = AX_CALLBACK_2(Character::stopCharacter, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(_keyboardListener, this);

    return true;
}

void Character::update(float delta)
{
    // Kiểm tra trạng thái của các phím nhấn và di chuyển nhân vật mượt mà
    ax::Vec2 movement(0, 0);  // Mặc định không di chuyển

    // Kiểm tra các phím đang được nhấn và thay đổi hướng di chuyển
    if (pressedKeys.count(EventKeyboard::KeyCode::KEY_LEFT_ARROW))
    {
        movement.x = -_speed * delta;  // Di chuyển sang trái
    }
    if (pressedKeys.count(EventKeyboard::KeyCode::KEY_RIGHT_ARROW))
    {
        movement.x = _speed * delta;  // Di chuyển sang phải
    }
    if (pressedKeys.count(EventKeyboard::KeyCode::KEY_UP_ARROW))
    {
        movement.y = _speed * delta;  // Di chuyển lên
    }
    if (pressedKeys.count(EventKeyboard::KeyCode::KEY_DOWN_ARROW))
    {
        movement.y = -_speed * delta;  // Di chuyển xuống
    }

    // Cập nhật vị trí nhân vật
    setPosition(getPosition() + movement);
}

void Character::setSpeed(float speed)
{
    _speed = speed;
}

float Character::getSpeed() const
{
    return _speed;
}

void Character::moveCharacter(EventKeyboard::KeyCode keyCode, Event* event)
{
    // Thêm phím vào danh sách các phím đang nhấn
    pressedKeys.insert(keyCode);
}

void Character::stopCharacter(EventKeyboard::KeyCode keyCode, Event* event)
{
    // Xóa phím ra khỏi danh sách khi nhả phím
    pressedKeys.erase(keyCode);
}
