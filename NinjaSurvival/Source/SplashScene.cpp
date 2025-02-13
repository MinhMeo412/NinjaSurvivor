//#include "Utils.h"
#include "SplashScene.h"
//#include "MainScene.h"

using namespace ax;

bool SplashScene::init()
{
    if (!Scene::init())
    {
        return false;
    }

    auto visibleSize = _director->getVisibleSize();
    auto origin = _director->getVisibleOrigin();
    auto safeArea = _director->getSafeAreaRect();
    auto safeOrigin = safeArea.origin;

    // Add a background image
    auto label = Label::createWithTTF("This Is Splash Scene", "fonts/Marker Felt.ttf", 24);
    label->setPosition(visibleSize.width / 2 + origin.x, visibleSize.height / 2 + origin.y);
    this->addChild(label);

    //this->scheduleOnce(AX_SCHEDULE_SELECTOR(SplashScene::goToMainScene), DISPLAY_TIME_SPLASH_SCENE);

    return true;
}

void SplashScene::goToMainScene(float dt)
{
//    auto scene = utils::createInstance<MainScene>();
//
//    Director::getInstance()->replaceScene(TransitionFade::create(TRANSITION_TIME,scene));
}
