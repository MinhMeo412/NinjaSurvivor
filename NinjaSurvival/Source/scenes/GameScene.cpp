#include "Utils.h"
#include "GameScene.h"
#include "MainScene.h"
#include "components/VelocityComponent.h"

using namespace ax;

GameScene::GameScene() : SceneBase("GameScene") {}

bool GameScene::init()
{
    if (!SceneBase::init())
    {
        return false;
    }

    // Close button
    closeItem = Utils::createMenuItem("CloseNormal.png", "CloseSelected.png",
                                      AX_CALLBACK_1(GameScene::menuCloseCallback, this), Vec2(100, 100));

    float x = safeOrigin.x + safeArea.size.width - closeItem->getContentSize().width / 2;
    float y = safeOrigin.y + closeItem->getContentSize().height / 2;
    closeItem->setPosition(Vec2(x, y));

    // Create menu
    auto menu = Menu::create(closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 10);

    // Border
    auto drawNode = DrawNode::create();
    drawNode->setPosition(Vec2(0, 0));
    addChild(drawNode);
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4F::BLUE);





    //Khởi tạo systemManager
    systemManager = SystemManager::getInstance();

    // Cập nhật scene cho MapSystem (chỉ khởi tạo một lần cho Map)
    if (auto mapSystem = systemManager->getSystem<MapSystem>())
    {
        mapSystem->setScene(this);
    }

    //Add thêm system khác
    systemManager->addSystem(std::make_unique<JoystickSystem>());

    //Gọi init các system chỉ thuộc khởi tạo mà không cần update
    systemManager->initSystems(this);





    // update
    scheduleUpdate();

    return true;
}

void GameScene::update(float dt)
{
    //systemManager->update(dt);
}

void GameScene::menuCloseCallback(ax::Object* sender)
{
    auto scene = utils::createInstance<MainScene>();
    _director->replaceScene(scene);
}
