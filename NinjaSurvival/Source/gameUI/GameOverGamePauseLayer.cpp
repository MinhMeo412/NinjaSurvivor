#include "Utils.h"
#include "GameOverGamePauseLayer.h"
#include "scenes/MainScene.h"
#include "scenes/GameScene.h"
#include "AudioManager.h"
#include "systems/ShopSystem.h"
#include "GameSceneUILayer.h"

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
    std::string panelSprite = isPlayerDead ? "UI/GameOver.png" : "UI/Pause.png";  // Thay bằng tên sprite của bạn
    auto panel              = Sprite::create(panelSprite);
    if (!panel)
    {
        AXLOG("Failed to create panel sprite: %s", panelSprite.c_str());
        return;
    }
    panel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
    this->addChild(panel, 20);  // z-order 20

    // Tạo vector chứa các menu item
    Vector<MenuItem*> menuItems;

    // Tạo nút Quit (luôn hiển thị)
    Vec2 quitPos;
    if (isPlayerDead)
    {
        // Khi player chết: đặt quitButton ở giữa panel
        quitPos = Vec2(panel->getContentSize().width / 2, panel->getContentSize().height / 2);
    }
    else
    {
        // Khi pause: giữ vị trí hiện tại
        quitPos = Vec2(panel->getContentSize().width / 2, panel->getContentSize().height * 2 / 9);
    }
    quitButton = Utils::createMenuItem("UI/buttonQuit.png", "UI/buttonQuit.png",
                                       AX_CALLBACK_1(GameOverGamePauseLayer::onQuitGame, this), quitPos);
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
            Vec2(panel->getContentSize().width / 2, panel->getContentSize().height * 5.5 / 9));
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
        // music click
        AudioManager::getInstance()->playSound("button_click", false, 1.0f, "click");

        gameScene->scheduleUpdate();  // Tiếp tục update của GameScene
    }
    this->removeFromParentAndCleanup(true);  // Xóa layer
}

void GameOverGamePauseLayer::onQuitGame(ax::Object* sender)
{
    // Vô hiệu hóa toàn bộ sự kiện đầu vào
    _director->getEventDispatcher()->setEnabled(false);

    // Get the GameScene (parent)
    auto gameScene = this->getParent();
    if (gameScene)
    {
        // Find GameSceneUILayer among the children of GameScene
        GameSceneUILayer* uiLayer = nullptr;
        for (auto* child : gameScene->getChildren())
        {
            uiLayer = dynamic_cast<GameSceneUILayer*>(child);
            if (uiLayer)
                break;
        }

        if (uiLayer)
        {
            // Get collectedCoin from GameSceneUILayer
            float collectedCoin = uiLayer->collectedCoin;

            // Update coins in ShopSystem
            auto shopSystem = ShopSystem::getInstance();
            int currentCoins = shopSystem->getCoins();
            int newCoins = currentCoins + static_cast<int>(collectedCoin);
            shopSystem->setCoins(newCoins);

            AXLOG("Saved %d collected coins to ShopSystem. Total coins: %d", static_cast<int>(collectedCoin), newCoins);
        }
        else
        {
            AXLOG("Error: Could not find GameSceneUILayer to retrieve collectedCoin");
        }

        // Lên lịch kích hoạt lại EventDispatcher trên GameScene
        gameScene->scheduleOnce([this](float) {
            _director->getEventDispatcher()->setEnabled(true);
            AXLOG("EventDispatcher re-enabled after transition");
        }, TRANSITION_TIME, "enableInput");
    }
    else
    {
        AXLOG("Error: Could not find GameScene parent");
        // Nếu không tìm thấy GameScene, kích hoạt lại ngay để tránh khóa ứng dụng
        _director->getEventDispatcher()->setEnabled(true);
    }

    // Play button click sound
    AudioManager::getInstance()->playSound("button_click", false, 1.0f, "click");
    AudioManager::getInstance()->stopSound("gamebackground");  // Dừng âm thanh của game

    // Xóa layer
    this->removeFromParentAndCleanup(true);

    // Chuyển về MainScene với hiệu ứng chuyển cảnh
    auto scene = utils::createInstance<MainScene>();
    Director::getInstance()->replaceScene(scene);
    this->scheduleOnce([this](float) { Director::getInstance()->getEventDispatcher()->setEnabled(true); },
                       TRANSITION_TIME,
                       "enableInput");
}
