#include "Utils.h"
#include "GameScene.h"
#include "MainScene.h"

using namespace ax;

GameScene::GameScene() : SceneBase("GameScene") {}

bool GameScene::init()
{
    if (!SceneBase::init())
    {
        return false;
    }

    

    // Close button
    closeItem = Utils::createMenuItem("CloseNormal.png", "CloseSelected.png",
                                      AX_CALLBACK_1(GameScene::menuCloseCallback, this), Vec2(100, 100));

    float x = safeOrigin.x + safeArea.size.width - closeItem->getContentSize().width / 2;
    float y = safeOrigin.y + closeItem->getContentSize().height / 2;
    closeItem->setPosition(Vec2(x, y));

    // Create menu
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 10);

    // Border
    auto drawNode = DrawNode::create();
    drawNode->setPosition(Vec2(0, 0));
    addChild(drawNode);
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4F::BLUE);


    //Khởi tạo object GameWorld
    gameWorld = std::make_unique<GameWorld>();


    //Khởi tạo systemManager
    systemManager = SystemManager::getInstance();

    //Gọi init các system
    systemManager->initSystems(this, gameWorld.get());


    // update
    scheduleUpdate();

    return true;
}

void GameScene::update(float dt)
{
    //Gọi đến update của toàn bộ các system có update
    systemManager->update(dt);
}

void GameScene::menuCloseCallback(ax::Object* sender)
{
    auto scene = utils::createInstance<MainScene>();
    _director->replaceScene(scene);
}

//Kiểm tra
// AXLOG("Reference count: %d", sprite->getReferenceCount());


//Code cũ để đối chiếu animation
//#ifndef __CHARACTER_H__
//#    define __CHARACTER_H__
//
//#    include "axmol.h"
//
//class Character : public ax::Sprite
//{
//public:
//    static Character* create(const std::string& spriteSheetPlist, const std::string& spriteFrameName);
//
//    virtual bool init(const std::string& spriteSheetPlist, const std::string& spriteFrameName);
//    virtual void update(float delta);
//
//    virtual void update(ax::Vec2 direction, float dt);
//
//    void setSpeed(float speed);
//    float getSpeed() const;
//
//private:
//    void createAnimations();
//    void runAnimation(int tag, const std::string& animationName);
//    void moveCharacter(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
//    void stopCharacter(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
//
//    // To move using the keyboard
//    // void moveCharacter(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
//    // void stopCharacter(ax::EventKeyboard::KeyCode keyCode, ax::Event* event);
//
//    ax::Animation* createAnimation(const std::string& format, int frameCount, float delay);
//
//    float _speed = 150.0f;
//    ax::Vec2 _velocity;
//    ax::Action* _currentAction        = nullptr;
//    static const int ACTION_TAG_LEFT  = 1;
//    static const int ACTION_TAG_RIGHT = 2;
//    static const int ACTION_TAG_UP    = 3;
//    static const int ACTION_TAG_DOWN  = 4;
//
//    ax::EventListenerKeyboard* _keyboardListener = nullptr;
//
//    ax::Animation* _downAnimation  = nullptr;
//    ax::Animation* _upAnimation    = nullptr;
//    ax::Animation* _leftAnimation  = nullptr;
//    ax::Animation* _rightAnimation = nullptr;
//
//    std::unordered_map<ax::EventKeyboard::KeyCode, bool> keyStates;
//};
//
//#endif  // __CHARACTER_H__

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
//    createAnimations();
//
//    return true;
//}

//void Character::update(Vec2 direction, float dt)
//{
//    bool isMoving = false;
//    Vec2 movement(0, 0);  // Mặc định không di chuyển
//    float adjustedSpeed = _speed / sqrt(2.0f);
//
//    if (keyStates[ax::EventKeyboard::KeyCode::KEY_LEFT_ARROW] && keyStates[ax::EventKeyboard::KeyCode::KEY_DOWN_ARROW])
//        if (direction.x < 0 && direction.y < 0)  // Di chuyển chéo trái xuống
//        {
//            movement.x = -adjustedSpeed * dt;
//            movement.y = -adjustedSpeed * dt;
//            runAnimation(ACTION_TAG_LEFT, "left");
//            // AXLOG("Move left down");
//            isMoving = true;
//        }
//        else if (keyStates[ax::EventKeyboard::KeyCode::KEY_LEFT_ARROW] &&
//                 keyStates[ax::EventKeyboard::KeyCode::KEY_UP_ARROW])
//            else if (direction.x < 0 && direction.y > 0)  // Di chuyển chéo trái lên
//            {
//                movement.x = -adjustedSpeed * dt;
//                movement.y = adjustedSpeed * dt;
//                runAnimation(ACTION_TAG_LEFT, "left");
//                // AXLOG("Move left up");
//                isMoving = true;
//            }
//        else if (keyStates[ax::EventKeyboard::KeyCode::KEY_RIGHT_ARROW] &&
//                 keyStates[ax::EventKeyboard::KeyCode::KEY_DOWN_ARROW])
//            else if (direction.x > 0 && direction.y < 0)  // Di chuyển chéo phải xuống
//            {
//                movement.x = adjustedSpeed * dt;
//                movement.y = -adjustedSpeed * dt;
//                runAnimation(ACTION_TAG_RIGHT, "right");
//                // AXLOG("Move right down");
//                isMoving = true;
//            }
//        else if (keyStates[ax::EventKeyboard::KeyCode::KEY_RIGHT_ARROW] &&
//                 keyStates[ax::EventKeyboard::KeyCode::KEY_UP_ARROW])
//            else if (direction.x > 0 && direction.y > 0)  // Di chuyển chéo phải lên
//            {
//                movement.x = adjustedSpeed * dt;
//                movement.y = adjustedSpeed * dt;
//                runAnimation(ACTION_TAG_RIGHT, "right");
//                // AXLOG("Move right up");
//                isMoving = true;
//            }
//        else if (keyStates[ax::EventKeyboard::KeyCode::KEY_LEFT_ARROW])
//            else if (direction.x < 0)  // Di chuyển trái
//            {
//                movement.x = -_speed * dt;
//                runAnimation(ACTION_TAG_LEFT, "left");
//                // AXLOG("Move left");
//                isMoving = true;
//            }
//        else if (keyStates[ax::EventKeyboard::KeyCode::KEY_RIGHT_ARROW])
//            else if (direction.x > 0)  // Di chuyển phải
//            {
//                movement.x = _speed * dt;
//                runAnimation(ACTION_TAG_RIGHT, "right");
//                // AXLOG("Move right");
//                isMoving = true;
//            }
//        else if (keyStates[ax::EventKeyboard::KeyCode::KEY_UP_ARROW])
//            else if (direction.y > 0)  // Di chuyển lên
//            {
//                movement.y = _speed * dt;
//                runAnimation(ACTION_TAG_UP, "up");
//                // AXLOG("Move up");
//                isMoving = true;
//            }
//        else if (keyStates[ax::EventKeyboard::KeyCode::KEY_DOWN_ARROW])
//            else if (direction.y < 0)  // Di chuyển xuống
//            {
//                movement.y = -_speed * dt;
//                runAnimation(ACTION_TAG_DOWN, "down");
//                // AXLOG("Move down");
//                isMoving = true;
//            }
//
//    //// Cập nhật vị trí nhân vật
//    if (movement != Vec2::ZERO)
//    {
//        setPosition(getPosition() + movement);
//    }
//    else if (!isMoving)  // Nếu không di chuyển và không có animation đang chạy
//    {
//        stopAllActions();  // Dừng tất cả các hành động khi không di chuyển
//        // Nếu bạn muốn có animation idle, bạn có thể chạy một animation idle ở đây.
//        // Có thể chạy một animation idle ở đây.
//    }
//}

//void Character::runAnimation(int tag, const std::string& animationName)
//{
//    if (_currentAction == nullptr || _currentAction->getTag() != tag)
//    {
//        if (_currentAction != nullptr)
//        {
//            stopAction(_currentAction);  // Dừng hành động trước đó nếu có
//        }
//        // Tạo và chạy animation di chuyển
//        _currentAction =
//            RepeatForever::create(Animate::create(AnimationCache::getInstance()->getAnimation(animationName)));
//        _currentAction->setTag(tag);
//        runAction(_currentAction);
//    }
//}

//void Character::createAnimations()
//{
//    _downAnimation  = createAnimation("./character_down%d", 3, 0.2f);
//    _upAnimation    = createAnimation("./character_up%d", 3, 0.2f);
//    _leftAnimation  = createAnimation("./character_left%d", 3, 0.2f);
//    _rightAnimation = createAnimation("./character_right%d", 3, 0.2f);
//
//    AnimationCache::getInstance()->addAnimation(_downAnimation, "down");
//    AnimationCache::getInstance()->addAnimation(_upAnimation, "up");
//    AnimationCache::getInstance()->addAnimation(_leftAnimation, "left");
//    AnimationCache::getInstance()->addAnimation(_rightAnimation, "right");
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
//        {
//            frames.pushBack(frame);
//        }
//        else
//        {
//            AXLOG("Frame not found: %s", frameName.c_str());
//        }
//    }
//
//    if (frames.empty())
//    {
//        AXLOG("No frames found for animation: %s", format.c_str());
//        return nullptr;  // Trả về nullptr nếu không có frame nào
//    }
//
//    return Animation::createWithSpriteFrames(frames, delay);
//}
