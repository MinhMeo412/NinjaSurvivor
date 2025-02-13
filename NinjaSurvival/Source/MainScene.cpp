#include "Utils.h"
#include "MainScene.h"
//#include "GameScene.h"

using namespace ax;

bool MainScene::init()
{
    if (!SceneBase::init())
    {
        return false;
    }

    float itemSpacing = 50.0;
    //Play button
    auto playItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",
                                          AX_CALLBACK_1(MainScene::menuPlayCallback, this));

    if (playItem == nullptr || playItem->getContentSize().width <= 0 || playItem->getContentSize().height <= 0)
    {
        Utils::problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        int itemOrderNum = 0;
        float x = safeOrigin.x + visibleSize.width/2;
        float y = safeOrigin.y + visibleSize.height / 2 - itemOrderNum * itemSpacing;
        playItem->setPosition(Vec2(x, y));
        auto label1 = Label::createWithTTF("Play button", "fonts/Marker Felt.ttf", 24);
        label1->setPosition(playItem->getPosition() + Vec2(100,0));
        this->addChild(label1, 1);
    }

    //Close button
    auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",
        AX_CALLBACK_1(MainScene::menuCloseCallback, this));

    if (closeItem == nullptr || closeItem->getContentSize().width <= 0 || closeItem->getContentSize().height <= 0)
    {
        Utils::problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
    }
    else
    {
        int itemOrderNum = 1;
        float x = safeOrigin.x + visibleSize.width / 2;
        float y = safeOrigin.y + visibleSize.height / 2 - itemOrderNum * itemSpacing;
        closeItem->setPosition(Vec2(x, y));
        auto label2 = Label::createWithTTF("Exit button", "fonts/Marker Felt.ttf", 24);
        label2->setPosition(closeItem->getPosition() + Vec2(100,0));
        this->addChild(label2, 1);
    }

    auto menu = Menu::create(playItem, closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);


    auto touchListener = EventListenerTouchAllAtOnce::create();
    touchListener->onTouchesBegan = AX_CALLBACK_2(MainScene::onTouchesBegan, this);
    touchListener->onTouchesMoved = AX_CALLBACK_2(MainScene::onTouchesMoved, this);
    touchListener->onTouchesEnded = AX_CALLBACK_2(MainScene::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);


    auto label = Label::createWithTTF("Ninja Survival Title", "fonts/Marker Felt.ttf", 24);
    if (label == nullptr)
    {
        Utils::problemLoading("'fonts/Marker Felt.ttf'");
    }
    else
    {
        label->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - label->getContentSize().height));
        this->addChild(label, 1);
    }

    return true;
}


void MainScene::onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        AXLOG("onTouchesBegan detected, X:%f  Y:%f", t->getLocation().x, t->getLocation().y);
    }
}

void MainScene::onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        AXLOG("onTouchesMoved detected, X:%f  Y:%f", t->getLocation().x, t->getLocation().y);
    }
}

void MainScene::onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    for (auto&& t : touches)
    {
        AXLOG("onTouchesEnded detected, X:%f  Y:%f", t->getLocation().x, t->getLocation().y);
    }
}


//Remove this button when release
void MainScene::menuCloseCallback(ax::Object* sender)
{
    _director->end();
}

void MainScene::menuPlayCallback(ax::Object* sender)
{
    _director->end();
//    auto scene = utils::createInstance<GameScene>();
//
//    _director->replaceScene(TransitionFade::create(0.5f, scene));
}

