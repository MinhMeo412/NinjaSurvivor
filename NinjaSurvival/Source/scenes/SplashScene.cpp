#include "Utils.h"
#include "SplashScene.h"
#include "LoadingScene.h"

using namespace ax;

SplashScene::SplashScene() : SceneBase("SplashScene") {}

bool SplashScene::init()
{
    if (!SceneBase::init())
    {
        return false;
    }

    auto visibleSize = _director->getVisibleSize();
    auto origin      = _director->getVisibleOrigin();

    auto background = Sprite::create("UI/background3.png");
    background->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
    this->addChild(background, 0);

    auto titleSprite = Sprite::create("UI/NeoTitle1.png");
    titleSprite->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height * 2 / 3));
    this->addChild(titleSprite, 1);

    this->scheduleOnce(AX_SCHEDULE_SELECTOR(SplashScene::goToLoadingScene), DISPLAY_TIME_SPLASH_SCENE);

    return true;
}

void SplashScene::update(float dt) {}

void SplashScene::goToLoadingScene(float dt)
{
    auto scene = utils::createInstance<LoadingScene>();

    Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME,scene));
}
