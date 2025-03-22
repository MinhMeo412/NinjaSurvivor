#include "Utils.h"
#include "GameSceneUILayer.h"
#include "GameOverGamePauseLayer.h"
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
