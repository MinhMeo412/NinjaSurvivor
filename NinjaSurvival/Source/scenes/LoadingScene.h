#ifndef __LOADING_SCENE_H__
#define __LOADING_SCENE_H__

#include "axmol.h"
#include "SceneBase.h"

class LoadingScene : public SceneBase
{
public:
    LoadingScene();

    bool init() override;
    void update(float dt) override;

private:
    ax::Label* loadingLabel;
    bool isLoaded = false;

    void createLoadingLabel();
    void onLoadComplete(float dt);
    void menuPlayCallback(ax::Object* sender);
};

#endif  // __LOADING_SCENE_H__
