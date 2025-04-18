#include "Utils.h"
#include "GameOverGamePauseLayer.h"
#include "GameSceneUILayer.h"
#include "scenes/MainScene.h"
#include "scenes/GameScene.h"
#include "AudioManager.h"
#include "systems/ShopSystem.h"
#include "systems/TimeSystem.h"

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
    Vec2 origin      = _director->getVisibleOrigin();
    Rect safeArea    = _director->getSafeAreaRect();
    Vec2 safeOrigin  = safeArea.origin;

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

    // Tạo label và sprite cho coin
    coinLabel = ax::Label::createWithTTF("0", "fonts/Pixelpurl-0vBPP.ttf", 24);
    
    auto coinSprite = Sprite::create("UI/coin.png");
    
    float coinLabelX = safeOrigin.x + safeArea.size.width - coinLabel->getContentSize().width -
                       coinSprite->getContentSize().width - 17;  // Margin 20
    float coinY = 640 - 20;                                      // Dưới thanh XP
    coinLabel->setPosition(coinLabelX, coinY);
    coinLabel->setAlignment(ax::TextHAlignment::RIGHT);
    this->addChild(coinLabel, 5);

    coinSprite->setPosition(
        coinLabelX + coinLabel->getContentSize().width / 2 + coinSprite->getContentSize().width / 2 + 7,
        coinY);  // Margin 5 giữa label và sprite
    coinSprite->setScale(2);
    this->addChild(coinSprite, 5);

    // Enemy kill count label
    enemyKillCountLabel = ax::Label::createWithTTF("0", "fonts/Pixelpurl-0vBPP.ttf", 24);
    auto skullSprite    = Sprite::create("UI/skull.png");
    
    float killLabelX = coinLabelX;  // Cùng cột với coinLabel
    float killY      = coinY - 30;  // Dưới coinLabel
    enemyKillCountLabel->setPosition(killLabelX, killY);
    enemyKillCountLabel->setAlignment(ax::TextHAlignment::RIGHT);
    this->addChild(enemyKillCountLabel, 5);

    skullSprite->setPosition(
        killLabelX + enemyKillCountLabel->getContentSize().width / 2 + skullSprite->getContentSize().width / 2 + 5,
        killY);                   // Margin 5
    skullSprite->setScale(0.8f);  // Thu nhỏ nếu cần
    this->addChild(skullSprite, 5);

    timerLabel = ax::Label::createWithTTF("00:00", "fonts/Pixelpurl-0vBPP.ttf", 24);
    timerLabel->setPosition(ax::Vec2(180, 620));  // Xem lại vị trí theo UI
    this->addChild(timerLabel, 5);             // Gán vào uiLayer
    setTimeTemp();

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

        //gameScene->scheduleUpdate();  // Tiếp tục update của GameScene

        SystemManager::getInstance()->setUpdateState(true);
    }
    this->removeFromParentAndCleanup(true);  // Xóa layer
}

void GameOverGamePauseLayer::onQuitGame(ax::Object* sender)
{
    Director::getInstance()->getEventDispatcher()->setEnabled(false);
    
    // music click
    AudioManager::getInstance()->playSound("button_click", false, 1.0f, "click");
    AudioManager::getInstance()->stopSound("gamebackground");  // dừng âm thanh của game

    this->removeFromParentAndCleanup(true);  // Xóa layer

    // Lấy số coin thu thập được từ GameSceneUILayer
    int collectedCoin = 0;
    auto currentScene = SystemManager::getInstance()->getCurrentScene();
    if (currentScene)
    {
        auto uiLayer = currentScene->getChildByName<GameSceneUILayer*>("UILayer");
        if (uiLayer)
        {
            collectedCoin = uiLayer->getCollectedCoin();
        }
        else
        {
            // Nếu không tìm thấy bằng tên, thử tìm trong các node con
            for (auto* child : currentScene->getChildren())
            {
                if ((uiLayer = dynamic_cast<GameSceneUILayer*>(child)))
                {
                    collectedCoin = uiLayer->getCollectedCoin();
                    break;
                }
            }
            if (!uiLayer)
            {
                AXLOG("Lỗi: Không tìm thấy GameSceneUILayer");
            }
        }
    }
    else
    {
        AXLOG("Lỗi: Không tìm thấy currentScene");
    }

    // Cập nhật số coin trong ShopSystem
    auto shopSystem  = ShopSystem::getInstance();
    float currentCoins = shopSystem->getCoins();
    float newCoins     = currentCoins + collectedCoin;  // Tính tổng số coin mới
    shopSystem->setCoins(newCoins);                   // Cập nhật số coin mới

    // Push scene mới
    auto scene = utils::createInstance<MainScene>();
    Director::getInstance()->pushScene(scene);

    // Lưu tham chiếu đến scene cũ
    Scene* gameScene = SystemManager::getInstance()->getCurrentScene();
    gameScene->retain();  // Giữ scene cũ để tránh bị giải phóng sớm

    // Do trên android xóa scene quá lâu nên k sử dụng replace
    // Lên lịch để pop scene cũ và cleanup
    scene->scheduleOnce([gameScene](float) {
        if (gameScene)
        {
            gameScene->onExit();  // Gọi onExit
            gameScene->cleanup();  // Gọi cleanup
            gameScene->release();  // Giải phóng
        }
        AXLOG("Xóa gameSccene");
        Director::getInstance()->getEventDispatcher()->setEnabled(true);
    }, 0.5f, "pop_old_scene");
}
void GameOverGamePauseLayer::setTimeTemp()
{
    auto currentScene = SystemManager::getInstance()->getCurrentScene();
    auto uiLayer    = currentScene->getChildByName<GameSceneUILayer*>("UILayer");

    auto timeSystem = SystemManager::getInstance()->getSystem<TimeSystem>();
    float timeSet   = timeSystem->getElapsedTime();

    int minutes = static_cast<int>(timeSet) / 60;
    int seconds = static_cast<int>(timeSet) % 60;

    // %02d in ra số nguyên d với ít nhất 2 chữ số (bổ sung số 0 nếu thiếu)
    timerLabel->setString(ax::StringUtils::format("%02d:%02d", minutes, seconds));
}
