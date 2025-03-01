#ifndef __SPLASH_SCENE_H__
#define __SPLASH_SCENE_H__

#include "axmol.h"
#include "SceneBase.h"

class SplashScene : public SceneBase
{
public:
    SplashScene();

    bool init() override;
    void update(float dt) override;

private:
    void goToLoadingScene(float dt);
};

#endif  // __SPLASH_SCENE_H__
