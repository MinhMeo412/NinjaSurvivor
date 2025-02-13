#ifndef __SPLASH_SCENE_H__
#define __SPLASH_SCENE_H__

#include "axmol.h"
//#include "SceneBase.h"

class SplashScene : public ax::Scene
{
public:
    bool init() override;
    void goToMainScene(float dt);
};

#endif  // __SPLASH_SCENE_H__
