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

    auto backgroundUpper = LayerColor::create(Color4B(0, 0, 0, 180));
    backgroundUpper->setContentSize(Size(360, 300));  // Kích thước nền mờ
    backgroundUpper->setPosition(Vec2(0, 580));       // Vị trí trên màn hình
    this->addChild(backgroundUpper, 0);

    auto backgroundLower = LayerColor::create(Color4B(0, 0, 0, 180));
    backgroundLower->setContentSize(Size(360, 300));
    backgroundLower->setPosition(Vec2(0, -300));     
    this->addChild(backgroundLower, 0);

    // Nút Pause
    pauseButton  = Utils::createMenuItem("UI/pauseButton.png", "UI/pauseButton.png",
                                         AX_CALLBACK_1(GameSceneUILayer::gamePauseCallback, this), Vec2(0, 0));
    float pauseX = safeOrigin.x + safeArea.size.width - pauseButton->getContentSize().width / 2 - 10;  // Góc phải
    float pauseY = safeOrigin.y + safeArea.size.height * 2 / 3;
    pauseButton->setScale(1.8);  // 2/3 từ dưới lên
    pauseButton->setPosition(Vec2(pauseX, pauseY));

    // Tạo menu
    auto menu = Menu::create(pauseButton, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 10);

    // Vẽ viền vùng an toàn
    auto drawNode = DrawNode::create();
    drawNode->setPosition(Vec2(0, 0));
    this->addChild(drawNode, 20);
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4F::BLUE);

    // Thanh XP
    xpBar      = Sprite::create("XPBar.png");
    xpBarUnder = Sprite::create("XPBarUnder.png");
    if (!xpBar || !xpBarUnder)
    {
        AXLOG("Không thể tạo xpBar hoặc xpBarUnder");
        return false;
    }
    xpBar->setAnchorPoint(Vec2(0, 0.5f));                                                  // Cố định bên trái
    xpBarUnder->setAnchorPoint(Vec2(0, 0.5f));                                             // Cố định bên trái
    float xpY = safeOrigin.y + safeArea.size.height - xpBar->getContentSize().height / 2;  // Sát mép trên
    float xpX = safeOrigin.x;                                                              // Căn trái
    xpBar->setPosition(xpX, xpY);
    xpBarUnder->setPosition(xpX, xpY);
    this->addChild(xpBar, 2);
    this->addChild(xpBarUnder, 1);

    // Tạo label và sprite cho coin
    coinLabel     = ax::Label::createWithTTF("0", "fonts/Pixelpurl-0vBPP.ttf", 24);
    if (!coinLabel)
    {
        AXLOG("Không thể tạo coinLabel");
        return false;
    }
    auto coinSprite = Sprite::create("coin.png");
    if (!coinSprite)
    {
        AXLOG("Không thể tạo coinSprite");
        return false;
    }
    float coinLabelX = safeOrigin.x + safeArea.size.width - coinLabel->getContentSize().width -
                       coinSprite->getContentSize().width - 17;  // Margin 20
    float coinY = 640 - 20;                                      // Dưới thanh XP
    coinLabel->setPosition(coinLabelX, coinY);
    coinLabel->setAlignment(ax::TextHAlignment::RIGHT);
    this->addChild(coinLabel, 5);

    coinSprite->setPosition(
        coinLabelX + coinLabel->getContentSize().width / 2 + coinSprite->getContentSize().width / 2 + 7,
        coinY);                  // Margin 5 giữa label và sprite
    coinSprite->setScale(2);  
    this->addChild(coinSprite, 5);

    //Enemy kill count label
    enemyKillCountLabel     = ax::Label::createWithTTF("0", "fonts/Pixelpurl-0vBPP.ttf", 24);
    auto skullSprite = Sprite::create("skull.png");
    if (!skullSprite)
    {
        AXLOG("Không thể tạo skullSprite");
        return false;
    }
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

    // Vị trí theo camera
    auto cameraSystem = SystemManager::getInstance()->getSystem<CameraSystem>();
    if (cameraSystem && cameraSystem->getCamera())
    {
        Vec2 cameraOffsetPos = cameraSystem->getCamera()->getPosition() - _director->getVisibleSize() / 2;
        this->setPosition(cameraOffsetPos);
    }

    // Thanh HP cho player
    hpBarRed  = Sprite::create("LifeBarMiniProgress.png");
    hpBarGray = Sprite::create("LifeBarMiniUnder.png");
    if (!hpBarRed || !hpBarGray)
    {
        AXLOG("Không thể tạo hpBarRed hoặc hpBarGray");
        return false;
    }
    hpBarRed->setAnchorPoint(Vec2(0, 0.5f));   // Cố định bên trái
    hpBarGray->setAnchorPoint(Vec2(0, 0.5f));  // Cố định bên trái
    this->addChild(hpBarRed, 2);
    this->addChild(hpBarGray, 1);


    // Level label
    levelLabel = ax::Label::createWithTTF("Level 1", "fonts/Marker Felt.ttf", 24);
    levelLabel->setPosition(ax::Vec2(180, xpBar->getPosition().y - 40));  // Xem lại vị trí
    this->addChild(levelLabel, 5);

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
    updateLevelLabel();
    updateEnemyKillCountLabel();
}

void GameSceneUILayer::increaseCoin(float coin)
{
    collectedCoin = collectedCoin + coin;
}
void GameSceneUILayer::increaseEnemyKillCount()
{
    enemyKillCount += 1;
}

void GameSceneUILayer::updateCoinLabel()
{
    coinLabel->setString(ax::StringUtils::format("%d", (int)collectedCoin));
}

void GameSceneUILayer::updateLevelLabel()
{
    int level = SystemManager::getInstance()->getSystem<LevelSystem>()->getLevel();
    levelLabel->setString(ax::StringUtils::format("Level %d", level));
}

void GameSceneUILayer::updateEnemyKillCountLabel()
{
    enemyKillCountLabel->setString(ax::StringUtils::format("%d", enemyKillCount));
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
