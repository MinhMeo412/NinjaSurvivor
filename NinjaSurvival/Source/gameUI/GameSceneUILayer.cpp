#include "Utils.h"
#include "GameSceneUILayer.h"
#include "scenes/MainScene.h"
#include "systems/SystemManager.h"
#include "systems/CameraSystem.h"

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
    closeButton = Utils::createMenuItem("CloseNormal.png", "CloseSelected.png",
                                        AX_CALLBACK_1(GameSceneUILayer::menuCloseCallback, this), Vec2(0, 0));
    float x     = safeOrigin.x + safeArea.size.width - closeButton->getContentSize().width / 2;
    float y     = safeOrigin.y + closeButton->getContentSize().height / 2;
    closeButton->setPosition(Vec2(x, y));

    // Create menu
    auto menu = Menu::create(closeButton, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 10);

    auto drawNode = DrawNode::create();
    drawNode->setPosition(Vec2(0, 0));
    this->addChild(drawNode,20);
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4F::BLUE);

    //Vị trí
    auto cameraSystem = SystemManager::getInstance()->getSystem<CameraSystem>();
    if (cameraSystem && cameraSystem->getCamera())
    {
        Vec2 cameraOffsetPos = cameraSystem->getCamera()->getPosition() - _director->getVisibleSize() / 2;
        this->setPosition(cameraOffsetPos);
    }

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
}


void GameSceneUILayer::menuCloseCallback(ax::Object* sender)
{
    auto scene = utils::createInstance<MainScene>();
    _director->replaceScene(scene);
}
