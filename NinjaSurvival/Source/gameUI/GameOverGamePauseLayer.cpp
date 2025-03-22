#include "Utils.h"
#include "GameOverGamePauseLayer.h"
#include "scenes/MainScene.h"
#include "scenes/GameScene.h"

using namespace ax;

GameOverGamePauseLayer::GameOverGamePauseLayer(bool isPlayerDead) : isPlayerDead(isPlayerDead) {}

GameOverGamePauseLayer* GameOverGamePauseLayer::create(bool isPlayerDead)
{
    auto layer = new (std::nothrow) GameOverGamePauseLayer(isPlayerDead);
    if (layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    AX_SAFE_DELETE(layer);
    return nullptr;
}

bool GameOverGamePauseLayer::init()
{
    if (!Layer::init())
        return false;

    // Tạo nền mờ
    auto background = LayerColor::create(Color4B(0, 0, 0, 128));  // Màu đen, độ mờ 50%
    this->addChild(background, 0);

    createUI();

    // Chặn sự kiện chạm phía dưới layer
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event) { return true; };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void GameOverGamePauseLayer::createUI()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // Label thông báo
    std::string message = isPlayerDead ? "Game Over" : "Paused";
    auto label          = Label::createWithTTF(message, "fonts/Marker Felt.ttf", 48);
    label->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    this->addChild(label, 2);

    // Nút Quit Game (luôn hiển thị)
    quitButton  = Utils::createMenuItem("CloseNormal.png", "CloseSelected.png",
                                        AX_CALLBACK_1(GameOverGamePauseLayer::onQuitGame, this),
                                        Vec2(visibleSize.width / 2, visibleSize.height / 2 - 250));
    auto label1 = Label::createWithTTF("Quit button", "fonts/Marker Felt.ttf", 24);
    label1->setPosition(quitButton->getPosition() + Vec2(100, 0));
    this->addChild(label1, 1);

    // Nút Return Game (chỉ hiển thị nếu player còn sống)
    if (!isPlayerDead)
    {
        returnButton = Utils::createMenuItem("CloseNormal.png", "CloseSelected.png",
                                             AX_CALLBACK_1(GameOverGamePauseLayer::onReturnGame, this),
                                             Vec2(visibleSize.width / 2, visibleSize.height / 2 - 200));
        auto label2  = Label::createWithTTF("Return button", "fonts/Marker Felt.ttf", 24);
        label2->setPosition(returnButton->getPosition() + Vec2(100, 0));
        this->addChild(label2, 1);
    }
    else
    {
        returnButton = nullptr;
    }

    // Tạo menu với các button hợp lệ
    auto menu = isPlayerDead ? Menu::create(quitButton, nullptr) : Menu::create(quitButton, returnButton, nullptr);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);
}

void GameOverGamePauseLayer::onReturnGame(ax::Object* sender)
{
    auto gameScene = this->getParent();
    if (gameScene)
    {
        gameScene->scheduleUpdate();  // Tiếp tục update của GameScene
    }
    this->removeFromParentAndCleanup(true);  // Xóa layer
}

void GameOverGamePauseLayer::onQuitGame(ax::Object* sender)
{
    this->removeFromParentAndCleanup(true);  // Xóa layer
    auto scene = utils::createInstance<MainScene>();
    Director::getInstance()->replaceScene(scene);  // Chuyển về MainScene
}
