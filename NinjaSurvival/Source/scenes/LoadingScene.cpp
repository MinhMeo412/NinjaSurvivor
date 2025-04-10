#include "Utils.h"
#include "LoadingScene.h"
#include "MainScene.h"
#include "systems/GameData.h"
#include "systems/ShopSystem.h"

using namespace ax;

LoadingScene::LoadingScene() : SceneBase("LoadingScene") {}

bool LoadingScene::init()
{
    if (!SceneBase::init())
    {
        return false;
    }

    createLoadingLabel();

    //Load game map data
    auto gameData = GameData::getInstance();
    if (!gameData->loadMapDataFromFile("maps.json"))
    {
        AXLOG("Failed to load map data");
        return false;
    }

    if (!gameData->loadEntityDataFromFile("entities.json"))
    {
        AXLOG("Failed to load entity data");
        return false;
    }

    // Load shop data
    auto shopData = ShopSystem::getInstance();
    if (shopData->isSaveGameExist())
    {
        if (!shopData->loadSaveGame())
            return false;
    }
    else
    {
        if (!shopData->createSaveGame())
            return false;
    }

    // Border
    auto drawNode = DrawNode::create();
    drawNode->setPosition(Vec2(0, 0));
    addChild(drawNode);
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4F::BLUE);

    this->scheduleOnce(AX_SCHEDULE_SELECTOR(LoadingScene::onLoadComplete), DELAY_LOADING_SCENE);

    return true;
}

void LoadingScene::update(float dt) {}

void LoadingScene::createLoadingLabel()
{
    loadingLabel = ax::Label::createWithTTF("Loading...", "fonts/Marker Felt.ttf", 30);
    loadingLabel->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    this->addChild(loadingLabel);

    // Hiệu ứng nhấp nháy
    auto blink = ax::RepeatForever::create(ax::Sequence::create(ax::FadeOut::create(0.5f), ax::FadeIn::create(0.5f), nullptr));
    loadingLabel->runAction(blink);
}

void LoadingScene::onLoadComplete(float dt)
{
    isLoaded = true;
    loadingLabel->setString("Press to Enter");
    loadingLabel->stopAllActions();
    loadingLabel->setOpacity(255);

    auto listener          = ax::EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](ax::Touch*, ax::Event*)
    {
        this->menuPlayCallback(nullptr);
        return true;
    };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);
}

void LoadingScene::menuPlayCallback(ax::Object* sender)
{
    auto scene = utils::createInstance<MainScene>();

    _director->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}
