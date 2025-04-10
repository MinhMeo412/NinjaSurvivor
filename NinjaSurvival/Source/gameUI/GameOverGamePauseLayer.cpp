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

    // Tạo panel sprite tương ứng với trạng thái (Game Over hoặc Paused)
    std::string panelSprite =
        isPlayerDead ? "UI/GameOver.png" : "UI/Pause.png";  // Thay bằng tên sprite của bạn
    auto panel = Sprite::create(panelSprite);
    if (!panel)
    {
        AXLOG("Failed to create panel sprite: %s", panelSprite.c_str());
        return;
    }
    panel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    this->addChild(panel, 20);  // z-order 1

    // Tạo vector chứa các menu item
    Vector<MenuItem*> menuItems;

    // Tạo nút Quit (luôn hiển thị)
    quitButton = Utils::createMenuItem(
        "UI/buttonQuit.png", "UI/buttonQuit.png", AX_CALLBACK_1(GameOverGamePauseLayer::onQuitGame, this),
                                     Vec2(panel->getContentSize().width / 2, panel->getContentSize().height * 2/9));
    if (quitButton)
    {
        menuItems.pushBack(quitButton);
    }
    else
    {
        AXLOG("Failed to create quitButton");
    }

    // Tạo nút Return (chỉ hiển thị nếu player còn sống)
    if (!isPlayerDead)
    {
        returnButton = Utils::createMenuItem(
            "UI/buttonReturn.png", "UI/buttonReturn.png", AX_CALLBACK_1(GameOverGamePauseLayer::onReturnGame, this),
            Vec2(panel->getContentSize().width / 2, panel->getContentSize().height * 5.5/9));
        if (returnButton)
        {
            menuItems.pushBack(returnButton);
        }
        else
        {
            AXLOG("Failed to create returnButton");
        }
    }
    else
    {
        returnButton = nullptr;
    }

    // Tạo menu từ danh sách menu items và thêm vào panel
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    panel->addChild(menu, 2);  // z-order 2, đè lên panel
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
