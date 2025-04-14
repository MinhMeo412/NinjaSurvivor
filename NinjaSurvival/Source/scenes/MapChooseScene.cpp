#include "MapChooseScene.h"
#include "Utils.h"
#include "MainScene.h"
#include "GameScene.h"
#include "GameLoadingScene.h"
#include "systems/GameData.h"
#include "systems/ShopSystem.h"
#include "systems/SystemManager.h"
#include "AudioManager.h"

using namespace ax;

MapChooseScene::MapChooseScene() : SceneBase("MapChooseScene") {}

// Khởi tạo scene, gọi reset hệ thống và thiết lập giao diện
bool MapChooseScene::init()
{
    if (!SceneBase::init())
    {
        return false;
    }
    SystemManager::getInstance()->resetSystems();
    menuUISetup();
    return true;
}

// Dự phòng cho cập nhật sau
void MapChooseScene::update(float dt) {}

// Chuyển về MainScene khi nhấn nút Close
void MapChooseScene::menuCloseCallback(Object* sender)
{
    // tắt nhạc nền
    AudioManager::getInstance()->playSound("button_click", false, 1.0f, "click");
    AudioManager::getInstance()->stopSound("background");

    _director->replaceScene(TransitionFade::create(TRANSITION_TIME, utils::createInstance<MainScene>()));
}

// Chuyển sang GameLoadingScene với GameScene khi nhấn nút Play
void MapChooseScene::menuPlayCallback(Object* sender)
{
    if (selectedMapName.empty())
    {
        AXLOG("Error: No map selected");
        return;
    }
    _director->getEventDispatcher()->setEnabled(false);

    GameData::getInstance()->setSelectedMap(selectedMapName);

    auto gameScene    = utils::createInstance<GameScene>();
    auto loadingScene = GameLoadingScene::create();

    loadingScene->setNextScene(gameScene);

    // Dừng nhạc nền
    AudioManager::getInstance()->playSound("button_click", false, 1.0f, "click");
    AudioManager::getInstance()->stopSound("background");

    _director->replaceScene(TransitionFade::create(TRANSITION_TIME, loadingScene));
    this->scheduleOnce([this](float) { _director->getEventDispatcher()->setEnabled(true); }, TRANSITION_TIME,
                       "enableInput");
}

// Thiết lập giao diện: background, panel, coin, nút Close, Play, Buy, và button map
void MapChooseScene::menuUISetup()
{
    float marginX = visibleSize.width * 0.05f;
    float marginY = visibleSize.height * 0.025f;

    // Tạo background
    auto background = Sprite::create("UI/background3.png");
    background->setPosition(origin + visibleSize / 2);
    this->addChild(background, 0);

    // Tạo panel map
    auto panel = Sprite::create("UI/panelMap.png");
    panel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2 - 10));
    this->addChild(panel, 2);

    // Vẽ viền debug
    auto drawNode = DrawNode::create();
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4F::BLUE);
    this->addChild(drawNode, 0);

    // Hiển thị coin
    auto shopData = ShopSystem::getInstance();
    auto coinLabel =
        Label::createWithTTF(StringUtils::format("%d", shopData->getCoins()), "fonts/Pixelpurl-0vBPP.ttf", 20);
    if (!coinLabel)
    {
        AXLOG("Không thể tạo coinLabel");
        return;
    }
    auto coinSprite = Sprite::create("coin.png");
    if (!coinSprite)
    {
        AXLOG("Không thể tạo coinSprite");
        return;
    }
    float coinLabelWidth  = coinLabel->getContentSize().width;
    float coinSpriteWidth = coinSprite->getContentSize().width;
    float totalWidth      = coinLabelWidth + coinSpriteWidth + 10.0f;
    float startPosX       = origin.x + visibleSize.width - marginX - totalWidth;
    float posY            = origin.y + visibleSize.height - marginY;
    coinLabel->setPosition(startPosX + coinLabelWidth / 2, posY);
    coinLabel->setAlignment(TextHAlignment::LEFT);
    this->addChild(coinLabel, 5, "coinLabel");
    coinSprite->setPosition(startPosX + coinLabelWidth + 10.0f + coinSpriteWidth / 2, posY);
    coinSprite->setScale(3);
    this->addChild(coinSprite, 5, "coinSprite");

    // Khởi tạo menu items
    Vector<MenuItem*> menuItems;

    // Tạo nút Close
    closeItem = Utils::createMenuItem("UI/buttonback.png", "UI/buttonback.png",
                                      AX_CALLBACK_1(MapChooseScene::menuCloseCallback, this),
                                      Vec2(origin.x + marginX, origin.y + visibleSize.height - marginY));
    menuItems.pushBack(closeItem);

    // Tính vị trí nút Play/Buy
    float panelRight  = panel->getPositionX() + panel->getContentSize().width / 2;
    float panelBottom = panel->getPositionY() - panel->getContentSize().height / 2;
    auto playSprite   = Sprite::create("UI/buttonPlayGame.png");
    float playX = panelRight - (playSprite->getContentSize().width / 2 * 0.8f) - panel->getContentSize().width * 0.075f;
    float playY =
        panelBottom + (playSprite->getContentSize().height / 2 * 0.8f) + panel->getContentSize().height * 0.03f;

    // Tạo nút Play
    auto playButton = dynamic_cast<MenuItemSprite*>(
        Utils::createButton("UI/buttonPlayGame.png", AX_CALLBACK_1(MapChooseScene::menuPlayCallback, this),
                            Vec2(playX, playY), 0.8f, false));
    if (!playButton)
    {
        AXLOG("Không thể tạo playButton");
        return;
    }
    menuItems.pushBack(playButton);

    // Tạo nút Buy
    auto buyButton = createBuyButton(playButton, panel, playX, playY);
    if (!buyButton)
    {
        AXLOG("Không thể tạo buyButton");
        return;
    }
    menuItems.pushBack(buyButton);

    // Tạo button map
    setupMapButtons(panel, playButton, buyButton, menuItems);

    // Tạo menu
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 3);
}

// Tạo nút Buy với logic mua map
MenuItemSprite* MapChooseScene::createBuyButton(MenuItemSprite* playButton, Node* panel, float x, float y)
{
    auto button = dynamic_cast<MenuItemSprite*>(
        Utils::createButton("UI/buttonBuy.png", [this, playButton, panel](Object* sender) {
        auto shopData    = ShopSystem::getInstance();
        auto gameData    = GameData::getInstance();
        int currentCoins = shopData->getCoins();
        int cost         = shopData->getCost("maps", selectedMapName);

        if (!selectedMapName.empty() && currentCoins >= cost)
        {
            shopData->setCoins(currentCoins - cost);
            shopData->setAvailable("maps", selectedMapName, true);
            gameData->setMapAvailable(selectedMapName, true);
            shopData->syncMapsWithGameData();

            if (selectedMapItem)
            {
                auto& maps = const_cast<std::unordered_map<std::string, MapData>&>(gameData->getMaps());
                if (maps.find(selectedMapName) != maps.end())
                {
                    maps[selectedMapName].available = true;
                }
                updateMapUI(selectedMapItem, selectedMapName, true, playButton, dynamic_cast<MenuItemSprite*>(sender));
            }

            if (auto* coinLabel = dynamic_cast<Label*>(this->getChildByName("coinLabel")))
            {
                coinLabel->setString(StringUtils::format("%d", shopData->getCoins()));
            }

            shopData->saveToFile(FileUtils::getInstance()->getWritablePath() + "savegame.json");
            AXLOG("Đã mở khóa map: %s", selectedMapName.c_str());
        }
        else
        {
            auto errorLabel = Label::createWithTTF("Không đủ coin!", "fonts/Pixelpurl-0vBPP.ttf", 20);
            errorLabel->setPosition(visibleSize / 2);
            this->addChild(errorLabel, 20);
            errorLabel->runAction(Sequence::create(FadeOut::create(2.0f), RemoveSelf::create(), nullptr));
            dynamic_cast<Node*>(sender)->setVisible(false);
        }
    }, Vec2(x, y), 0.8f, false));
    if (!button)
    {
        AXLOG("Không thể tạo buyButton");
        return nullptr;
    }
    return button;
}

// Cập nhật giao diện: nhãn map, nút Play/Buy dựa trên trạng thái map
void MapChooseScene::updateMapUI(MenuItemSprite* item,
                                 const std::string& mapName,
                                 bool isAvailable,
                                 MenuItemSprite* playButton,
                                 MenuItemSprite* buyButton)
{
    Utils::updateItemUI(item, nullptr, isAvailable);
    if (auto* label = dynamic_cast<Label*>(item->getChildByName("label")))
    {
        if (isAvailable)
        {
            label->setString(mapName);
        }
        else
        {
            auto shopData = ShopSystem::getInstance();
            int cost      = shopData->getCost("maps", mapName);
            label->setString(StringUtils::format("Locked\nCost: %d", cost));
        }
        label->setVisible(true);
    }
    buyButton->setVisible(!isAvailable);
    playButton->setVisible(isAvailable);
}

// Tạo các button map và thiết lập callback
void MapChooseScene::setupMapButtons(Node* panel,
                                     MenuItemSprite* playButton,
                                     MenuItemSprite* buyButton,
                                     Vector<MenuItem*>& menuItems)
{
    auto gameData    = GameData::getInstance();
    const auto& maps = gameData->getMaps();
    size_t numMaps   = maps.size();
    const int cols   = 1;
    float startX     = panel->getPositionX();
    float startY     = panel->getPositionY() + panel->getContentSize().height / 3;
    float yPos       = 100;
    int index        = 0;

    for (const auto& pair : maps)
    {
        const auto& map = pair.second;
        auto button =
            Utils::createMapButton(map.sprite, map.name, map.available, selectedMapName, selectedMapItem, playButton);
        if (!button)
        {
            AXLOG("Không thể tạo button map: %s", map.name.c_str());
            continue;
        }

        button->setCallback([this, mapName = map.name, playButton, buyButton](Object* sender) {
            selectedMapName = mapName;

            if (selectedMapItem && selectedMapItem != sender)
            {
                if (auto* oldLabel = dynamic_cast<Label*>(selectedMapItem->getChildByName("label")))
                {
                    oldLabel->setVisible(false);
                }
                if (auto* oldBorder = selectedMapItem->getChildByName("border"))
                {
                    oldBorder->setVisible(false);
                }
            }
            if (selectedMapItem)
            {
                selectedMapItem->setEnabled(true);
            }
            selectedMapItem = dynamic_cast<MenuItemSprite*>(sender);
            selectedMapItem->setEnabled(false);
            if (auto* border = selectedMapItem->getChildByName("border"))
            {
                border->setVisible(true);
            }

            auto gameData    = GameData::getInstance();
            bool isAvailable = gameData->getMaps().at(mapName).available;
            updateMapUI(selectedMapItem, mapName, isAvailable, playButton, buyButton);
        });

        float buttonY = startY + (button->getContentSize().height / 2) - yPos -
                        (index * (button->getContentSize().height * 0.4 + yPos));
        button->setPosition(Vec2(startX, buttonY));
        menuItems.pushBack(button);
        index++;
        yPos += 25;
    }
}
