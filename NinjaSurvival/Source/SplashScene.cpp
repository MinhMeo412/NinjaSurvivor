#include "SplashScene.h"
#include "MainScene.h"

using namespace ax;

// Print useful error message instead of segfaulting when files are not there.
static void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
    printf(
        "Depending on how you compiled you might have to add 'Content/' in front of filenames in "
        "MainScene.cpp\n");
}

// on "init" you need to initialize your instance
bool SplashScene::init()
{
    //////////////////////////////
    // 1. super init first
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

    
    this->scheduleOnce(AX_SCHEDULE_SELECTOR(SplashScene::goToMainScene), 2.0f);

    return true;
}

void SplashScene::goToMainScene(float dt)
{
    auto scene = utils::createInstance<MainScene>();

    Director::getInstance()->replaceScene(TransitionFade::create(2.0f,scene));
}
