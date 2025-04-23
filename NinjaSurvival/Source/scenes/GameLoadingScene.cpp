#include "Utils.h"
#include "GameLoadingScene.h"
#include "GameScene.h"

using namespace ax;

GameLoadingScene::GameLoadingScene() : SceneBase("GameLoadingScene") {}

GameLoadingScene* GameLoadingScene::create()
{
    GameLoadingScene* scene = new (std::nothrow) GameLoadingScene();
    if (scene && scene->init())
    {
        scene->autorelease();
        return scene;
    }
    AX_SAFE_DELETE(scene);
    return nullptr;
}

bool GameLoadingScene::init()
{
    if (!SceneBase::init())
    {
        return false;
    }

    createLoadingLabel();


    // Border
    auto drawNode = DrawNode::create();
    drawNode->setPosition(Vec2(0, 0));
    addChild(drawNode);
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4F::BLUE);

    scheduleUpdate();

    return true;
}

void GameLoadingScene::update(float dt)
{
    delayTimer += dt;
    if (delayTimer >= 1.5f)
    {
        if (nextScene)
        {
            ax::Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME, nextScene));
        }
        else
        {
            AXLOG("Error: No next scene set for GameLoadingScene");
        }
    }
}

void GameLoadingScene::setNextScene(ax::Scene* nextScene)
{
    this->nextScene = nextScene;
}

void GameLoadingScene::createLoadingLabel()
{
    loadingLabel = ax::Label::createWithTTF("Loading...", "fonts/Pixelpurl-0vBPP.ttf", 30);
    loadingLabel->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    this->addChild(loadingLabel);

    // Hiệu ứng nhấp nháy
    auto blink = ax::RepeatForever::create(ax::Sequence::create(ax::FadeOut::create(0.5f), ax::FadeIn::create(0.5f), nullptr));
    loadingLabel->runAction(blink);
}


