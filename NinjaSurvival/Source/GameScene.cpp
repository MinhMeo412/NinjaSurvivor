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

    //UI Layer
    UILayer = Layer::create();
    UILayer->setAnchorPoint(Vec2(1,1));
    this->addChild(UILayer, 5);


    auto drawNode = DrawNode::create();
    drawNode->setPosition(Vec2(0, 0));
    UILayer->addChild(drawNode, 10);
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4B::BLUE);


    //Close item
    auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", AX_CALLBACK_1(GameScene::goToMainScene, this));

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

    //Menu UI
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    UILayer->addChild(menu, 10);

    
    auto touchListener            = EventListenerTouchAllAtOnce::create();
    touchListener->onTouchesBegan = AX_CALLBACK_2(GameScene::onTouchesBegan, this);
    touchListener->onTouchesMoved = AX_CALLBACK_2(GameScene::onTouchesMoved, this);
    touchListener->onTouchesEnded = AX_CALLBACK_2(GameScene::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);




    //Tiled map
    /*auto map = TMXTiledMap::create("Map.tmx");
    map->setAnchorPoint(Vec2(0, 0));
    map->setPosition(Vec2(100, 100));
    this->addChild(map, 2);*/

    //Test tile map
    auto tileMap = new TMXTiledMap();
    tileMap->initWithTMXFile("Test.tmx");
    this->addChild(tileMap, 0);


    //Character
    character = Character::create("character.plist", "./character_down0");
    character->setPosition(200, 200);
    character->setScale(3.0f);
    this->addChild(character, 3);
    

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




void GameScene::update(float delta)
{
    character->update(delta);

    auto visibleSize = _director->getVisibleSize();

    auto camera = this->getCameras().front();
    camera->setPosition(character->getPosition());
    UILayer->setPosition(Vec2(camera->getPosition().x - visibleSize.width / 2, camera->getPosition().y - visibleSize.height / 2));
}

void GameScene::menuCloseCallback(ax::Object* sender)
{
    _director->popScene();
}

void GameScene::goToMainScene(ax::Object* sender)
{
    auto scene = utils::createInstance<MainScene>();

    Director::getInstance()->replaceScene(TransitionFade::create(0.5f, scene));
}
