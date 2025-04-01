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

    float marginX = visibleSize.width * 0.05f;  // 5% of screen width as margin from the left
    float marginY = visibleSize.height * 0.025f;

    // Position the back button in the top-left corner

    closeItem   = Utils::createMenuItem("UI/buttonback.png", "UI/buttonback.png",
                                        AX_CALLBACK_1(MapChooseScene::menuCloseCallback, this),
                                      Vec2(origin.x + marginX, origin.y + visibleSize.height - marginY));
    menuItems.pushBack(closeItem);

    // Tính toán lại vị trí của playItem để nằm ở góc dưới phải của panel
    float panelRight  = panel->getPositionX() + (panel->getContentSize().width / 2);
    float panelBottom = panel->getPositionY() - (panel->getContentSize().height / 2);


    // Play button
    auto playSprite         = Sprite::create("UI/buttonPlayGame.png");
    auto playSelectedSprite = Sprite::create("UI/buttonPlayGame.png");
    playItem                = MenuItemSprite::create(playSprite, playSelectedSprite, nullptr,
                                                     AX_CALLBACK_1(MapChooseScene::menuPlayCallback, this));
    // Đặt playItem ở góc dưới phải của panel
    float playX = panelRight - (playItem->getContentSize().width / 2) -
                  (panel->getContentSize().width * 0.05);  // Cách mép phải 5% chiều rộng panel
    float playY = panelBottom + (playItem->getContentSize().height / 2) +
                  (panel->getContentSize().height * 0.05);  // Cách mép dưới 5% chiều cao panel

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
    float iconSpacingX = panel->getContentSize().width * 0.15;   // 5% chiều rộng panel
    float iconSpacingY = panel->getContentSize().height * 0.06  ;  // 5% chiều cao panel

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
        float startX           = panel->getPositionX() - (totalButtonWidth / 2) + (button->getContentSize().width / 2);
        float startY           = panel->getPositionY() + (panel->getContentSize().height / 3.1) +
                       (button->getContentSize().height / 2) - iconSpacingY;

        float buttonX = startX + col * (button->getContentSize().width + iconSpacingX);
        float buttonY = startY - row * (button->getContentSize().height + iconSpacingY);

        button->setPosition(Vec2(buttonX, buttonY));
        button->setScale(0.4);
        menuItems.pushBack(button);
        index++;
    }

    // Tạo Menu chứa tất cả button
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 3);
}
