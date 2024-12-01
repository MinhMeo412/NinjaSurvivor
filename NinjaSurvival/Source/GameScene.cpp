#include "GameScene.h"
#include "MainScene.h"
#include "Character.h"

using namespace ax;

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf(
        "Depending on how you compiled you might have to add 'Content/' in front of filenames in "
        "GameScene.cpp\n");
}

// on "init" you need to initialize your instance
bool GameScene::init()
{
    //////////////////////////////
    // 1. super init first
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = _director->getVisibleSize();
    auto origin      = _director->getVisibleOrigin();
    auto safeArea    = _director->getSafeAreaRect();
    auto safeOrigin  = safeArea.origin;


    UILayer = Layer::create();
    UILayer->setAnchorPoint(Vec2(1,1));
    this->addChild(UILayer, 5);
    /////////////////////////////
    // 2. add a menu item with "X" image, which is clicked to quit the program
    //    you may modify it.

    // add a "close" icon to exit the progress. it's an autorelease object
    auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",
                                           AX_CALLBACK_1(GameScene::goToMainScene, this));

    if (closeItem == nullptr || closeItem->getContentSize().width <= 0 || closeItem->getContentSize().height <= 0)
    {
        problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        float x = safeOrigin.x + safeArea.size.width - closeItem->getContentSize().width / 2;
        float y = safeOrigin.y + closeItem->getContentSize().height / 2;
        closeItem->setPosition(Vec2(x, y));
    }

    // create menu, it's an autorelease object
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    UILayer->addChild(menu, 10);

    /////////////////////////////
    // 3. add your codes below...

    // Some templates (uncomment what you  need)
    auto touchListener            = EventListenerTouchAllAtOnce::create();
    touchListener->onTouchesBegan = AX_CALLBACK_2(GameScene::onTouchesBegan, this);
    touchListener->onTouchesMoved = AX_CALLBACK_2(GameScene::onTouchesMoved, this);
    touchListener->onTouchesEnded = AX_CALLBACK_2(GameScene::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    // auto mouseListener           = EventListenerMouse::create();
    // mouseListener->onMouseMove   = AX_CALLBACK_1(GameScene::onMouseMove, this);
    // mouseListener->onMouseUp     = AX_CALLBACK_1(GameScene::onMouseUp, this);
    // mouseListener->onMouseDown   = AX_CALLBACK_1(GameScene::onMouseDown, this);
    // mouseListener->onMouseScroll = AX_CALLBACK_1(GameScene::onMouseScroll, this);
    //_eventDispatcher->addEventListenerWithSceneGraphPriority(mouseListener, this);

    auto drawNode = DrawNode::create();
    drawNode->setPosition(Vec2(0, 0));
    UILayer->addChild(drawNode, 10);

    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4B::BLUE);







    //Map
    auto map = TMXTiledMap::create("Map.tmx");
    map->setAnchorPoint(Vec2(0, 0));
    map->setPosition(Vec2(100, 100));
    this->addChild(map, 2);


    //Character
    //auto spriteFrameCache = SpriteFrameCache::getInstance();
    //spriteFrameCache->addSpriteFramesWithFile("character.plist");

    //character = Sprite::createWithSpriteFrameName("./character_down0");  // Frame đầu tiên
    //character->setPosition(Vec2(200, 200));
    //character->setScale(1.0f);
    //this->addChild(character,3);

    /*character = Character::create("character.plist", "./character_down0");
    character->setPosition(200, 200);
    this->addChild(character, 3);*/
    
       character = Character::create("character1.png");  // Đảm bảo đường dẫn tới hình ảnh đúng
    if (character)
    {
        addChild(character,3);  // Thêm character vào scene
    }


    // scheduleUpdate() is required to ensure update(float) is called on every loop
    scheduleUpdate();

    return true;
}





void GameScene::onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        AXLOG("onTouchesBegan detected, X:%f  Y:%f", t->getLocation().x, t->getLocation().y);
    }
}

void GameScene::onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        AXLOG("onTouchesMoved detected, X:%f  Y:%f", t->getLocation().x, t->getLocation().y);
    }
}

void GameScene::onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        AXLOG("onTouchesEnded detected, X:%f  Y:%f", t->getLocation().x, t->getLocation().y);
    }
}

void GameScene::onMouseDown(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    AXLOG("onMouseDown detected, Key: %d", static_cast<int>(e->getMouseButton()));
}

void GameScene::onMouseUp(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    AXLOG("onMouseUp detected, Key: %d", static_cast<int>(e->getMouseButton()));
}

void GameScene::onMouseMove(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    AXLOG("onMouseMove detected, X:%f  Y:%f", e->getCursorX(), e->getCursorY());
}

void GameScene::onMouseScroll(Event* event)
{
    EventMouse* e = static_cast<EventMouse*>(event);
    AXLOG("onMouseScroll detected, X:%f  Y:%f", e->getScrollX(), e->getScrollY());
}



void GameScene::update(float delta)
{
    //// Kiểm tra nếu phím mũi tên nào đang được nhấn và cập nhật vị trí nhân vật
    //movement = Vec2(0, 0);  // Mặc định không di chuyển

    //// Kiểm tra các phím đang được nhấn và thay đổi hướng di chuyển
    //if (pressedKeys.count(EventKeyboard::KeyCode::KEY_LEFT_ARROW))
    //{
    //    movement.x = -150 * delta;  // Di chuyển sang trái, sử dụng deltaTime để di chuyển mượt mà
    //}
    //if (pressedKeys.count(EventKeyboard::KeyCode::KEY_RIGHT_ARROW))
    //{
    //    movement.x = 150 * delta;  // Di chuyển sang phải, sử dụng deltaTime để di chuyển mượt mà
    //}
    //if (pressedKeys.count(EventKeyboard::KeyCode::KEY_UP_ARROW))
    //{
    //    movement.y = 150 * delta;  // Di chuyển lên, sử dụng deltaTime để di chuyển mượt mà
    //}
    //if (pressedKeys.count(EventKeyboard::KeyCode::KEY_DOWN_ARROW))
    //{
    //    movement.y = -150 * delta;  // Di chuyển xuống, sử dụng deltaTime để di chuyển mượt mà
    //}


    character->update(delta);


    auto visibleSize = _director->getVisibleSize();
    // Cập nhật vị trí nhân vật dựa trên hướng di chuyển
    //character->setPosition(character->getPosition() + movement);
    auto camera = this->getCameras().front();
    camera->setPosition(character->getPosition());
    UILayer->setPosition(
        Vec2(camera->getPosition().x - visibleSize.width / 2, camera->getPosition().y - visibleSize.height / 2));



    switch (_gameState)
    {
    case GameState::init:
    {
        _gameState = GameState::update;
        break;
    }

    case GameState::update:
    {
        /////////////////////////////
        // Add your codes below...like....
        //
        // UpdateJoyStick();
        // UpdatePlayer();
        // UpdatePhysics();
        // ...
        break;
    }

    case GameState::pause:
    {
        /////////////////////////////
        // Add your codes below...like....
        //
        // anyPauseStuff()

        break;
    }

    case GameState::menu1:
    {  /////////////////////////////
        // Add your codes below...like....
        //
        // UpdateMenu1();
        break;
    }

    case GameState::menu2:
    {  /////////////////////////////
        // Add your codes below...like....
        //
        // UpdateMenu2();
        break;
    }

    case GameState::end:
    {  /////////////////////////////
        // Add your codes below...like....
        //
        // CleanUpMyCrap();
        menuCloseCallback(this);
        break;
    }

    }  // switch
}

void GameScene::menuCloseCallback(ax::Object* sender)
{

    _director->popScene();
    // Close the axmol game scene and quit the application
    //_director->end();

    /*To navigate back to native iOS screen(if present) without quitting the application  ,do not use
     * _director->end() as given above,instead trigger a custom event created in RootViewController.mm
     * as below*/

    // EventCustom customEndEvent("game_scene_close_event");
    //_eventDispatcher->dispatchEvent(&customEndEvent);
}

void GameScene::goToMainScene(ax::Object* sender)
{
    auto scene = utils::createInstance<MainScene>();

    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, scene));
}
