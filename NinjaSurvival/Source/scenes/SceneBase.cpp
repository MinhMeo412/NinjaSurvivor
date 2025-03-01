#include "SceneBase.h"
// #include "AudioManager.h"

using namespace ax;

int SceneBase::s_sceneID = 1000;

bool SceneBase::init()
{
    if (!Scene::init())
    {
        return false;
    }

    visibleSize = _director->getVisibleSize();
    origin      = _director->getVisibleOrigin();
    safeArea    = _director->getSafeAreaRect();
    safeOrigin  = safeArea.origin;

    return true;
}

void SceneBase::update(float dt) {}

SceneBase::SceneBase(const std::string& sceneName) : sceneName(sceneName)
{
    sceneID = ++s_sceneID;
    AXLOGD("{}: ctor: #{}", sceneName, sceneID);
}

SceneBase::~SceneBase()
{
    AXLOGD("~{}: dtor: #{}", sceneName, sceneID);
}
