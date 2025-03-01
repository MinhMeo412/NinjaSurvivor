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

    // Add a background image
    auto label = Label::createWithTTF("This Is Splash Scene", "fonts/Marker Felt.ttf", 24);
    label->setPosition(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);
    this->addChild(label);

    this->scheduleOnce(AX_SCHEDULE_SELECTOR(SplashScene::goToLoadingScene), DISPLAY_TIME_SPLASH_SCENE);

    return true;
}

void SplashScene::update(float dt) {}

void SplashScene::goToLoadingScene(float dt)
{
    auto scene = utils::createInstance<LoadingScene>();

    Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME,scene));
}
