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
    // Border
    auto drawNode = DrawNode::create();
    drawNode->setPosition(Vec2(0, 0));
    addChild(drawNode);
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4F::BLUE);

    //Menu vector chứa các menuItems
    Vector<MenuItem*> menuItems;

    // Close button
    closeItem = Utils::createMenuItem("CloseNormal.png", "CloseSelected.png",
                                      AX_CALLBACK_1(MapChooseScene::menuCloseCallback, this), Vec2(1100, 100));
    auto label2 = Label::createWithTTF("Exit button", "fonts/Marker Felt.ttf", 24);
    label2->setPosition(closeItem->getPosition() + Vec2(100, 0));
    this->addChild(label2, 1);
    menuItems.pushBack(closeItem);

    // Play button
    auto playSprite = Sprite::create("CloseNormal.png");
    auto playSelectedSprite = Sprite::create("CloseSelected.png");
    playItem                = MenuItemSprite::create(playSprite, playSelectedSprite, nullptr,
                                                     AX_CALLBACK_1(MapChooseScene::menuPlayCallback, this));
    playItem->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2, 100));
    playItem->setVisible(false);
    menuItems.pushBack(playItem);

    

    // Tạo các button chọn map
    auto gameData    = GameData::getInstance();
    const auto& maps = gameData->getMaps();

    float xPos = 200;
    for (const auto& pair : maps)
    {
        const auto& map = pair.second;
        auto button =
            Utils::createMapButton(map.sprite, map.name, map.available, selectedMapName, selectedMapItem, playItem);
        button->setPosition(Vec2(xPos, 400));

        menuItems.pushBack(button);
        xPos += 200;
    }
    
    // Tạo Menu chứa tất cả button
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);
}
