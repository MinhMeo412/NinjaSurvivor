#include "Utils.h"
#include "GameSceneUILayer.h"
#include "GameOverGamePauseLayer.h"
#include "systems/SystemManager.h"
#include "systems/CameraSystem.h"
#include "systems/SpawnSystem.h"
#include "systems/HealthSystem.h"
#include "systems/LevelSystem.h"
#include "systems/WeaponSystem.h"


using namespace ax;

GameSceneUILayer *GameSceneUILayer::create()
{
    GameSceneUILayer *layer = new (std::nothrow) GameSceneUILayer();
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
    Vec2 origin = _director->getVisibleOrigin();
    Rect safeArea = _director->getSafeAreaRect();
    Vec2 safeOrigin = safeArea.origin;

    // Tải texture atlas từ gameSceneUI.plist
    //SpriteFrameCache::getInstance()->addSpriteFramesWithFile("gameSceneUI.plist");

    // Tạo SpriteBatchNode để chứa tất cả các sprite
    /*batchNode = SpriteBatchNode::create("gameSceneUI.png");
    if (!batchNode)
    {
        AXLOG("Không thể tạo SpriteBatchNode");
        return false;
    }
    this->addChild(batchNode, 1);*/

    // Nền mờ trên (giữ nguyên như mã bạn cung cấp)
    auto backgroundUpper = LayerColor::create(Color4B(0, 0, 0, 180));
    backgroundUpper->setContentSize(Size(360, 300));
    backgroundUpper->setPosition(Vec2(0, 580));
    this->addChild(backgroundUpper, 0);

    // Nền mờ dưới (giữ nguyên như mã bạn cung cấp)
    auto backgroundLower = LayerColor::create(Color4B(0, 0, 0, 180));
    backgroundLower->setContentSize(Size(360, 300));
    backgroundLower->setPosition(Vec2(0, -300));
    this->addChild(backgroundLower, 0);

    // Nút Pause
    pauseButton = Utils::createMenuItem("UI/pauseButton.png", "UI/pauseButton.png",
                                        AX_CALLBACK_1(GameSceneUILayer::gamePauseCallback, this), Vec2(0, 0));
    float pauseX = safeOrigin.x + safeArea.size.width - pauseButton->getContentSize().width / 2 - 10;
    float pauseY = safeOrigin.y + (safeArea.size.height * 2 / 3) - 10; // Đặt sát mép trên safeArea
    pauseButton->setScale(2);
    pauseButton->setPosition(Vec2(pauseX, pauseY));

    // Tạo menu
    auto menu = Menu::create(pauseButton, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 10);

    // Thanh XP
    xpBar = Sprite::create("XPBar.png");
    xpBarUnder = Sprite::create("XPBarUnder.png");
    if (!xpBar || !xpBarUnder)
    {
        AXLOG("Không thể tạo xpBar hoặc xpBarUnder");
        return false;
    }
    xpBar->setAnchorPoint(Vec2(0, 0.5f));
    xpBarUnder->setAnchorPoint(Vec2(0, 0.5f));
    float xpY = safeOrigin.y + safeArea.size.height -
                xpBar->getContentSize().height / 2; // Đặt sát mép trên safeArea, đồng bộ với GameOverGamePauseLayer
    float xpX = safeOrigin.x;
    xpBar->setPosition(xpX, xpY);
    xpBarUnder->setPosition(xpX, xpY);
    this->addChild(xpBar, 2);
    this->addChild(xpBarUnder, 1);

    // Tạo label và sprite cho coin
    coinLabel = ax::Label::createWithTTF("0", "fonts/Pixelpurl-0vBPP.ttf", 24);
    if (!coinLabel)
    {
        AXLOG("Không thể tạo coinLabel");
        return false;
    }
    auto coinSprite = Sprite::create("UI/coin.png");
    if (!coinSprite)
    {
        AXLOG("Không thể tạo coinSprite");
        return false;
    }
    float coinY = xpY - 20; // Dưới xpBar 30 pixel, đồng bộ với GameOverGamePauseLayer
    coinLabel->setAnchorPoint(Vec2(1, 0.5));
    coinLabel->setAlignment(ax::TextHAlignment::RIGHT);
    float coinLabelX = safeOrigin.x + safeArea.size.width - coinSprite->getContentSize().width - 17;
    coinLabel->setPosition(coinLabelX, coinY);
    coinSprite->setPosition(coinLabelX + coinSprite->getContentSize().width / 2 + 7, coinY);
    coinSprite->setScale(0.8);
    this->addChild(coinLabel, 5);
    this->addChild(coinSprite, 5);

    // Enemy kill count label
    enemyKillCountLabel = ax::Label::createWithTTF("0", "fonts/Pixelpurl-0vBPP.ttf", 24);
    if (!enemyKillCountLabel)
    {
        AXLOG("Không thể tạo enemyKillCountLabel");
        return false;
    }
    auto skullSprite = Sprite::create("UI/skull.png");
    if (!skullSprite)
    {
        AXLOG("Không thể tạo skullSprite");
        return false;
    }
    float killY = coinY - 20; // Dưới coinLabel 30 pixel, đồng bộ với GameOverGamePauseLayer
    enemyKillCountLabel->setAnchorPoint(Vec2(1, 0.5));
    enemyKillCountLabel->setAlignment(ax::TextHAlignment::RIGHT);
    float killLabelX = coinLabelX;
    enemyKillCountLabel->setPosition(killLabelX, killY);
    skullSprite->setPosition(killLabelX + skullSprite->getContentSize().width / 2 + 7, killY);
    skullSprite->setScale(0.8f);
    this->addChild(enemyKillCountLabel, 5);
    this->addChild(skullSprite, 5);

    // Vị trí theo camera
    auto cameraSystem = SystemManager::getInstance()->getSystem<CameraSystem>();
    if (cameraSystem && cameraSystem->getCamera())
    {
        Vec2 cameraOffsetPos = cameraSystem->getCamera()->getPosition() - _director->getVisibleSize() / 2;
        this->setPosition(cameraOffsetPos);
    }

    // Thanh HP cho player
    hpBarRed = Sprite::create("LifeBarMiniProgress.png");
    hpBarGray = Sprite::create("LifeBarMiniUnder.png");
    if (!hpBarRed || !hpBarGray)
    {
        AXLOG("Không thể tạo hpBarRed hoặc hpBarGray");
        return false;
    }
    hpBarRed->setAnchorPoint(Vec2(0, 0.5f));
    hpBarGray->setAnchorPoint(Vec2(0, 0.5f));
    this->addChild(hpBarRed, 2);
    this->addChild(hpBarGray, 1);

    // Level label
    levelLabel = ax::Label::createWithTTF("Level 1", "fonts/Pixelpurl-0vBPP.ttf", 24);
    levelLabel->setPosition(
        ax::Vec2(safeOrigin.x + safeArea.size.width / 2, xpY - 40)); // Căn giữa ngang, dưới xpBar 40 pixel
    this->addChild(levelLabel, 5);


    

    // Tạo 8 sprite
    for (int i = 0; i < 8; ++i)
    {
        auto sprite = Sprite::create("iconWP/iconContain.png");

        // Tính vị trí cho mỗi sprite (2 hàng x 4 cột)
        float x = origin.x + (i % 4) * (sprite->getContentSize().width + 3) + 12;
        float y = origin.y + visibleSize.height - (i / 4) * (sprite->getContentSize().height + 3) - 24;

        sprite->setPosition(Vec2(x, y));
        sprite->setTag(i + 1);
        this->addChild(sprite);
    }

    updateInventoryUI();

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

// Cập nhật vị trí và máu của thanh HP
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

void GameSceneUILayer::gamePauseCallback(ax::Object *sender)
{
    auto pauseLayer = GameOverGamePauseLayer::create(false);
    if (pauseLayer)
    {
        auto gameScene = this->getParent();
        if (gameScene)
        {
            Vec2 layerPos = this->getPosition();
            pauseLayer->setPosition(layerPos);

            gameScene->addChild(pauseLayer, 1000);  // Thêm layer với z-order cao
            // gameScene->unscheduleUpdate();         // Dừng update của GameScene
            SystemManager::getInstance()->setUpdateState(false);
        }
    }
}

void GameSceneUILayer::bossAlert()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();

    // Tạo label "Boss Incoming"
    auto bossLabel = Label::createWithTTF("Boss Incoming", "fonts/Pixelpurl-0vBPP.ttf", 36);
    bossLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));

    bossLabel->setColor(Color3B::RED);  // Đặt màu đỏ cho label
    bossLabel->setOpacity(0);           // Bắt đầu ẩn để fade in
    this->addChild(bossLabel, 10);

    // Tạo hiệu ứng: FadeIn + Blink, sau đó xóa label
    auto fadeIn = FadeIn::create(0.5f);  // Xuất hiện mượt mà trong 0.5 giây
    auto blink = Blink::create(2.5f, 5); // Nhấp nháy 5 lần trong 2.5 giây
    auto remove = CallFunc::create([bossLabel]()
                                   { bossLabel->removeFromParentAndCleanup(true); });
    auto sequence = Sequence::create(fadeIn, blink, remove, nullptr);
    bossLabel->runAction(sequence);
}

void GameSceneUILayer::updateInventoryUI()
{
    // std::vector<std::string> wpList = SystemManager::getInstance()->getSystem<WeaponSystem>()->getPlayerInventory(1);
    // std::vector<std::string> buffList = SystemManager::getInstance()->getSystem<WeaponSystem>()->getPlayerInventory(0);

    //Test
    std::vector<std::string> wpList   = {"bigkunai", "bigkunai", "", ""};
    std::vector<std::string> buffList = {"health", "health", "", ""};

    // Duyệt qua 8 sprite (tag từ 1 đến 8)
    for (int i = 0; i < 8; ++i)
    {
        // Lấy sprite theo tag (từ 1 đến 8)
        auto sprite = dynamic_cast<ax::Sprite*>(this->getChildByTag(i + 1));
        if (!sprite)
            continue;  // Bỏ qua nếu không tìm thấy sprite

        // Hàng trên (i = 0, 1, 2, 3) dành cho wpList (vũ khí)
        if (i < 4)
        {
            // Kiểm tra nếu vị trí i có vũ khí tương ứng trong wpList
            if (i < wpList.size() && wpList[i] != "")
            {
                // Tạo sprite dựa trên tên file ảnh của vũ khí
                //std::string weaponIconPath = "iconWP/icon" + wpList[i] + ".png"; //Tên ảnh phải thêm từ icon đằng trước để phân biệt còn sử dụng Cache
                std::string weaponIconPath = "iconWP/" + wpList[i] + ".png";

                auto weaponSprite = ax::Sprite::create(weaponIconPath);
                if (weaponSprite)
                {
                    // Đặt sprite con ở giữa sprite gốc
                    weaponSprite->setPosition(sprite->getPosition());
                    this->addChild(weaponSprite, 1);  // Thêm sprite con với z-order cao hơn
                }
            }
            else
            {
                continue;
            }
        }
        // Hàng dưới (i = 4, 5, 6, 7) dành cho buffList (buff)
        else
        {
            // Kiểm tra nếu vị trí (i - 4) có buff tương ứng trong buffList
            if ((i - 4) < buffList.size() && buffList[i - 4] != "")
            {
                // Tạo sprite dựa trên tên file ảnh của vũ khí
                //std::string buffIconPath = "iconWP/icon" + buffList[i - 4] + ".png";
                std::string buffIconPath = "iconWP/" + buffList[i - 4] + ".png";

                auto buffSprite        = ax::Sprite::create(buffIconPath);
                if (buffSprite)
                {
                    // Đặt sprite con ở giữa sprite gốc
                    buffSprite->setPosition(sprite->getPosition());
                    this->addChild(buffSprite, 1);  // Thêm sprite con với z-order cao hơn
                }
            }
            else
            {
                continue;
            }
        }
    }
}
