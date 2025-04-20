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
    std::string panelSprite = isPlayerDead ? "UI/GameOver.png" : "UI/Pause.png";
    auto panel              = Sprite::create(panelSprite);
    if (!panel)
    {
        AXLOG("Failed to create panel sprite: %s", panelSprite.c_str());
        return;
    }
    panel->setPosition(Vec2(safeOrigin.x + safeArea.size.width / 2, safeOrigin.y + safeArea.size.height / 2));
    this->addChild(panel, 20);

    // Tạo label và sprite cho coin
    coinLabel = ax::Label::createWithTTF("0", "fonts/Pixelpurl-0vBPP.ttf", 24);
    if (!coinLabel)
    {
        AXLOG("Lỗi: Không thể tạo coinLabel");
        return;
    }
    auto coinSprite = Sprite::create("UI/coin.png");
    if (!coinSprite)
    {
        AXLOG("Lỗi: Không thể tạo coinSprite");
        return;
    }
    // Tính coinY dựa trên safeArea, đồng bộ với GameSceneUILayer
    float coinY = 635.000f - 20;  // Dưới xpBar 30 pixel, khớp với GameSceneUILayer
    coinLabel->setAnchorPoint(Vec2(1, 0.5));
    coinLabel->setAlignment(ax::TextHAlignment::RIGHT);
    getCoin();
    float coinLabelX = safeOrigin.x + safeArea.size.width - coinSprite->getContentSize().width - 17;
    coinLabel->setPosition(coinLabelX, coinY);
    coinSprite->setPosition(coinLabelX + coinSprite->getContentSize().width / 2 + 7, coinY);
    this->addChild(coinLabel, 5);
    coinSprite->setScale(0.8);
    this->addChild(coinSprite, 5);

    // Enemy kill count label
    enemyKillCountLabel = ax::Label::createWithTTF("0", "fonts/Pixelpurl-0vBPP.ttf", 24);
    if (!enemyKillCountLabel)
    {
        AXLOG("Lỗi: Không thể tạo enemyKillCountLabel");
        return;
    }
    auto skullSprite = Sprite::create("UI/skull.png");
    if (!skullSprite)
    {
        AXLOG("Lỗi: Không thể tạo skullSprite");
        return;
    }
    float killY = coinY - 20;  // Dưới coinLabel 30 pixel, khớp với GameSceneUILayer
    enemyKillCountLabel->setAnchorPoint(Vec2(1, 0.5));
    enemyKillCountLabel->setAlignment(ax::TextHAlignment::RIGHT);
    float killLabelX = coinLabelX;  // Cùng cột với coinLabel
    enemyKillCountLabel->setPosition(killLabelX, killY);
    skullSprite->setPosition(killLabelX + skullSprite->getContentSize().width / 2 + 7, killY);
    getKillCount();
    this->addChild(enemyKillCountLabel, 5);
    skullSprite->setScale(0.8f);  // Đúng, giữ nguyên
    this->addChild(skullSprite, 5);

    // Timer label
    timerLabel = ax::Label::createWithTTF("00:00", "fonts/Pixelpurl-0vBPP.ttf", 24);
    if (!timerLabel)
    {
        AXLOG("Lỗi: Không thể tạo timerLabel");
        return;
    }
    timerLabel->setPosition(ax::Vec2(safeOrigin.x + safeArea.size.width / 2, safeOrigin.y + safeArea.size.height - 20));
    this->addChild(timerLabel, 5);
    setTimeTemp();

    // Tạo vector chứa các menu item
    Vector<MenuItem*> menuItems;

    // Tạo nút Quit
    Vec2 quitPos = isPlayerDead ? Vec2(panel->getContentSize().width / 2, panel->getContentSize().height / 2)
                                : Vec2(panel->getContentSize().width / 2, panel->getContentSize().height * 2 / 9);
    quitButton   = Utils::createMenuItem("UI/buttonQuit.png", "UI/buttonQuit.png",
                                         AX_CALLBACK_1(GameOverGamePauseLayer::onQuitGame, this), quitPos);
    if (quitButton)
    {
        menuItems.pushBack(quitButton);
    }
    else
    {
        AXLOG("Failed to create quitButton");
    }

    // Tạo nút Return (chỉ khi pause)
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

    // Tạo menu từ danh sách menu items
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    panel->addChild(menu, 2);
}

ax::Layer* GameOverGamePauseLayer::getUILayer() const
{
    auto currentScene = SystemManager::getInstance()->getCurrentScene();
    if (!currentScene)
    {
        AXLOG("Lỗi: Không tìm thấy currentScene");
        return nullptr;
    }

    auto uiLayer = currentScene->getChildByName<GameSceneUILayer*>("UILayer");
    if (!uiLayer)
    {
        for (auto* child : currentScene->getChildren())
        {
            if ((uiLayer = dynamic_cast<GameSceneUILayer*>(child)))
                break;
        }
        if (!uiLayer)
        {
            AXLOG("Lỗi: Không tìm thấy GameSceneUILayer");
        }
    }
    return uiLayer;
}

void GameOverGamePauseLayer::getCoin()
{
    if (!coinLabel)
    {
        AXLOG("Lỗi: coinLabel chưa được khởi tạo");
        return;
    }

    int collectedCoin = 0;
    if (auto uiLayer = getUILayer())
    {
        collectedCoin = dynamic_cast<GameSceneUILayer*>(uiLayer)->getCollectedCoin();
    }
    coinLabel->setString(ax::StringUtils::format("%d", collectedCoin));
}

void GameOverGamePauseLayer::getKillCount()
{
    if (!enemyKillCountLabel)
    {
        AXLOG("Lỗi: enemyKillCountLabel chưa được khởi tạo");
        return;
    }

    int killCount = 0;
    if (auto uiLayer = getUILayer())
    {
        killCount = dynamic_cast<GameSceneUILayer*>(uiLayer)->getKillCount();
    }
    enemyKillCountLabel->setString(ax::StringUtils::format("%d", killCount));
}

void GameOverGamePauseLayer::setTimeTemp()
{
    float timeSet = SystemManager::getInstance()->getSystem<TimeSystem>()->getElapsedTime();
    int minutes   = static_cast<int>(timeSet) / 60;
    int seconds   = static_cast<int>(timeSet) % 60;
    timerLabel->setString(ax::StringUtils::format("%02d:%02d", minutes, seconds));
}

void GameOverGamePauseLayer::onReturnGame(ax::Object* sender)
{
    AudioManager::getInstance()->playSound("button_click", false, 1.0f, "click");
    SystemManager::getInstance()->setUpdateState(true);
    this->removeFromParentAndCleanup(true);
}

void GameOverGamePauseLayer::onQuitGame(ax::Object* sender)
{
    Director::getInstance()->getEventDispatcher()->setEnabled(false);
    AudioManager::getInstance()->playSound("button_click", false, 1.0f, "click");
    AudioManager::getInstance()->stopSound("gamebackground");

    this->removeFromParentAndCleanup(true);

    // Lấy số coin từ GameSceneUILayer
    int collectedCoin = 0;
    if (auto uiLayer = getUILayer())
    {
        collectedCoin = dynamic_cast<GameSceneUILayer*>(uiLayer)->getCollectedCoin();
        if (collectedCoin < 0)
        {
            AXLOG("Cảnh báo: collectedCoin âm (%d), đặt về 0", collectedCoin);
            collectedCoin = 0;
        }
    }

    // Cập nhật coin vào ShopSystem
    auto shopSystem    = ShopSystem::getInstance();
    float currentCoins = shopSystem->getCoins();
    float newCoins     = currentCoins + collectedCoin;
    shopSystem->setCoins(static_cast<int>(newCoins));

    // Chuyển sang MainScene
    auto scene = utils::createInstance<MainScene>();
    Director::getInstance()->pushScene(scene);

    // Xử lý xóa scene cũ
    Scene* gameScene = SystemManager::getInstance()->getCurrentScene();
    gameScene->retain();
    scene->scheduleOnce([gameScene](float) {
        if (gameScene)
        {
            gameScene->onExit();
            gameScene->cleanup();
            gameScene->release();
        }
        AXLOG("Xóa gameScene");
        Director::getInstance()->getEventDispatcher()->setEnabled(true);
    }, 0.5f, "pop_old_scene");
}
