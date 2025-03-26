#include "Utils.h"
#include "MapChooseScene.h"
#include "MainScene.h"
#include "GameScene.h"
#include "GameLoadingScene.h"
#include "systems/GameData.h"


using namespace ax;

MapChooseScene::MapChooseScene() : SceneBase("MapChooseScene") {}

bool MapChooseScene::init()
{
    if (!SceneBase::init())
    {
        return false;
    }

    systemManager = SystemManager::getInstance();
    systemManager->resetSystems();

    menuUISetup();

    return true;
}

void MapChooseScene::update(float dt) {}

void MapChooseScene::menuCloseCallback(ax::Object* sender)
{
    auto scene = utils::createInstance<MainScene>();
    _director->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}

void MapChooseScene::menuPlayCallback(ax::Object* sender)
{
    if (selectedMapName.empty())
    {
        AXLOG("Error: No map selected");
        return;
    }

    //Truyền dữ liệu tên map được chọn cho GameData lưu
    GameData::getInstance()->setSelectedMap(selectedMapName);

    // GameScene
    auto gameScene = utils::createInstance<GameScene>();

    // Tạo GameLoadingScene
    auto loadingScene = GameLoadingScene::create();

    // Gán GameScene làm nextScene
    loadingScene->setNextScene(gameScene);

    _director->replaceScene(TransitionFade::create(TRANSITION_TIME, loadingScene));
}

void MapChooseScene::menuUISetup()
{
    auto visibleSize = _director->getVisibleSize();
    auto origin      = _director->getVisibleOrigin();

    // Border
    auto drawNode = DrawNode::create();
    drawNode->setPosition(Vec2(0, 0));
    addChild(drawNode);
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4F::BLUE);

    auto background = Sprite::create("UI/background3.png");
    background->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
    this->addChild(background, 0);

    auto panel = Sprite::create("UI/panelMap.png");  // Panel chính
    panel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
    this->addChild(panel, 2);

    // Menu vector chứa các menuItems
    Vector<MenuItem*> menuItems;

    auto backButton = MenuItemImage::create("UI/buttonback.png", "UI/buttonback.png",
                                            AX_CALLBACK_1(MapChooseScene::menuCloseCallback, this));

    float marginX = visibleSize.width * 0.05;  // 5% of screen width as margin from the left
    float marginY = visibleSize.height * 0.001;

    // Position the back button in the top-left corner
    float backX = origin.x + marginX + (backButton->getContentSize().width / 2); // Center of the button, offset by margin
    float backY = origin.y + visibleSize.height - marginY - (backButton->getContentSize().height / 2);
    backButton->setPosition(Vec2(backX, backY));
    this->addChild(backButton, 6);


    float panelRight  = panel->getPositionX() + (panel->getContentSize().width / 2);
    float panelBottom = panel->getPositionY() - (panel->getContentSize().height / 2);

    float playX = panelRight - (panel->getContentSize().width * 0.1);  // Cách mép phải 10% chiều rộng panel
    float playY = panelBottom - (visibleSize.height * 0.05);           // Cách mép dưới 5% chiều cao màn hình

    // Đảm bảo nút không nằm ngoài màn hình
    playX = std::min(playX, visibleSize.width - 40.0f);  // Giả sử chiều rộng sau scale là 80, cách mép phải ít nhất 40
    playY = std::max(playY, 40.0f);

    // Play button
    auto playSprite         = Sprite::create("UI/buttonPlayGame.png");
    auto playSelectedSprite = Sprite::create("UI/buttonPlayGame.png");
    playItem                = MenuItemSprite::create(playSprite, playSelectedSprite, nullptr,
                                                     AX_CALLBACK_1(MapChooseScene::menuPlayCallback, this));
    playItem->setPosition(Vec2(playX, playY));
    playItem->setScale(0.8);
    playItem->setVisible(false);
    menuItems.pushBack(playItem);

    // Tạo các button chọn map
    auto gameData    = GameData::getInstance();
    const auto& maps = gameData->getMaps();

    // Đếm số lượng map
    int numMaps = maps.size();

    // Tính toán số cột và hàng
    const int cols = 2;                            // Số cột (có thể thay đổi nếu muốn)
    const int rows = (numMaps + cols - 1) / cols;  // Số hàng, làm tròn lên

    // Khoảng cách giữa các button
    float iconSpacingX = panel->getContentSize().width * 0.08;   // 5% chiều rộng panel
    float iconSpacingY = panel->getContentSize().height * 0.08;  // 5% chiều cao panel

    int index = 0;
    for (const auto& pair : maps)
    {
        const auto& map = pair.second;
        auto button =
            Utils::createMapButton(map.sprite, map.name, map.available, selectedMapName, selectedMapItem, playItem);

        // Tính vị trí button trong lưới
        int row = index / cols;  // Hàng
        int col = index % cols;  // Cột

        float totalButtonWidth = cols * button->getContentSize().width + (cols - 1) * iconSpacingX;
        float startX = panel->getPositionX() - (totalButtonWidth / 2) + (button->getContentSize().width / 2);
        float startY = panel->getPositionY() +
                       (panel->getContentSize().height /2.3) +
                       (button->getContentSize().height /2) - iconSpacingY;

        float buttonX = startX + col * (button->getContentSize().width + iconSpacingX);
        float buttonY = startY - row * (button->getContentSize().height + iconSpacingY);

        button->setPosition(Vec2(buttonX, buttonY));
        button->setScale(0.7);
        menuItems.pushBack(button);
        index++;
    }

    // Tạo Menu chứa tất cả button
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 3);
}
