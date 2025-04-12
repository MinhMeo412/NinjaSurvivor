#include "Utils.h"
#include "GameSceneUILayer.h"
#include "GameOverGamePauseLayer.h"
#include "systems/SystemManager.h"
#include "systems/CameraSystem.h"
#include "systems/SpawnSystem.h"
#include "systems/HealthSystem.h"
#include "systems/LevelSystem.h"


using namespace ax;

GameSceneUILayer* GameSceneUILayer::create()
{
    GameSceneUILayer* layer = new (std::nothrow) GameSceneUILayer();
    if (layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    AX_SAFE_DELETE(layer);
    return nullptr;
}

bool GameSceneUILayer::init()
{
    if (!Layer::init())
        return false;

    Vec2 visibleSize = _director->getVisibleSize();
    Vec2 origin      = _director->getVisibleOrigin();
    Rect safeArea    = _director->getSafeAreaRect();
    Vec2 safeOrigin  = safeArea.origin;

    // Close button
    pauseButton = Utils::createMenuItem("CloseNormal.png", "CloseSelected.png",
                                        AX_CALLBACK_1(GameSceneUILayer::gamePauseCallback, this), Vec2(0, 0));
    float x     = safeOrigin.x + safeArea.size.width - pauseButton->getContentSize().width / 2;
    float y     = safeOrigin.y + pauseButton->getContentSize().height / 2;
    pauseButton->setPosition(Vec2(x, y));

    // Create menu
    auto menu = Menu::create(pauseButton, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 10);

    auto drawNode = DrawNode::create();
    drawNode->setPosition(Vec2(0, 0));
    this->addChild(drawNode,20);
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4F::BLUE);

    // Tạo label để hiển thị coin
    collectedCoin = 0;
    coinLabel = ax::Label::createWithTTF("0", "fonts/Marker Felt.ttf", 24);
    coinLabel->setPosition(ax::Vec2(180, 600));  // Xem lại vị trí theo UI
    this->addChild(coinLabel, 5);                // Gán vào uiLayer

    //Vị trí
    auto cameraSystem = SystemManager::getInstance()->getSystem<CameraSystem>();
    if (cameraSystem && cameraSystem->getCamera())
    {
        Vec2 cameraOffsetPos = cameraSystem->getCamera()->getPosition() - _director->getVisibleSize() / 2;
        this->setPosition(cameraOffsetPos);
    }

    //Tạo thanh HP cho player
    hpBarRed = Sprite ::create("LifeBarMiniProgress.png");
    hpBarGray = Sprite ::create("LifeBarMiniUnder.png");
    hpBarRed->setAnchorPoint(Vec2(0, 0.5f));  //Cố định bên trái
    hpBarGray->setAnchorPoint(Vec2(0, 0.5f)); //Cố định bên trái
    this->addChild(hpBarRed, 2);
    this->addChild(hpBarGray, 1);

    //Tạo thanh XP
    xpBar = Sprite ::create("XPBar.png");
    xpBarUnder = Sprite ::create("XPBarUnder.png");
    xpBar->setAnchorPoint(Vec2(0, 0.5f));      // Cố định bên trái
    xpBarUnder->setAnchorPoint(Vec2(0, 0.5f));  // Cố định bên trái
    xpBar->setPosition(1,580);
    xpBarUnder->setPosition(0, 580);
    this->addChild(xpBar, 2);
    this->addChild(xpBarUnder, 1);

    return true;
}

void GameSceneUILayer::update(float dt)
{
    auto cameraSystem = SystemManager::getInstance()->getSystem<CameraSystem>();
    if (cameraSystem && cameraSystem->getCamera())
    {
        Vec2 cameraOffsetPos = cameraSystem->getCamera()->getPosition() - _director->getVisibleSize() / 2;
        this->setPosition(cameraOffsetPos);
    }

    updateHPBar();
    updateCoinLabel();
    updateXPBar();
}

void GameSceneUILayer::increaseCoin(float coin)
{
    collectedCoin = collectedCoin + coin;
}

void GameSceneUILayer::updateCoinLabel()
{
    //coinLabel->setString(std::to_string((int)collectedCoin));
    coinLabel->setString(ax::StringUtils::format("%d", (int)collectedCoin));
}

//Cập nhật vị trí và máu của thanh HP
void GameSceneUILayer::updateHPBar()
{
    // Lấy SpawnSystem từ SystemManager
    auto spawnSystem = SystemManager::getInstance()->getSystem<SpawnSystem>();
    if (!spawnSystem)
        return;

    // Lấy vị trí world của player
    ax::Vec2 playerWorldPos = spawnSystem->getPlayerPosition();

    // Điều chỉnh vị trí xuống dưới chân player
    playerWorldPos.x -= (hpBarRed->getContentSize().width / 2);
    playerWorldPos.y -= 12.0f;

    // Chuyển đổi sang tọa độ local trong UILayer
    ax::Vec2 localPos = this->convertToNodeSpace(playerWorldPos);
    hpBarGray->setPosition(localPos);
    hpBarRed->setPosition(localPos);

    auto healthSystem = SystemManager::getInstance()->getSystem<HealthSystem>();
    if (!healthSystem)
        return;
    // Cập nhật tỷ lệ thanh đỏ theo phần trăm máu còn lại
    hpBarRed->setScaleX(healthSystem->getPlayerCurrentHealth() / healthSystem->getPlayerMaxHealth());
}

void GameSceneUILayer::updateXPBar()
{
    auto levelSystem = SystemManager::getInstance()->getSystem<LevelSystem>();
    if (!levelSystem)
        return;

    // Cập nhật tỷ lệ thanh xp theo phần trăm
    xpBar->setScaleX(levelSystem->getCurrentXP() / levelSystem->getNeededXP());
}

void GameSceneUILayer::gamePauseCallback(ax::Object* sender)
{
    auto pauseLayer   = GameOverGamePauseLayer::create(false);
    if (pauseLayer)
    {
        auto gameScene = this->getParent();
        if (gameScene)
        {
            Vec2 layerPos = this->getPosition();
            pauseLayer->setPosition(layerPos);
            gameScene->addChild(pauseLayer, 1000);  // Thêm layer với z-order cao
            gameScene->unscheduleUpdate();         // Dừng update của GameScene
        }
    }
}
