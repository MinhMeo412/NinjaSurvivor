#ifndef __GAME_LOADING_SCENE_H__
#define __GAME_LOADING_SCENE_H__

#include "axmol.h"
#include "SceneBase.h"

class GameLoadingScene : public SceneBase
{
public:
    GameLoadingScene();

    static GameLoadingScene* create();
    bool init() override;
    void update(float dt) override;

    void setNextScene(ax::Scene* nextScene);

private:
    ax::Label* loadingLabel;
    bool isLoaded = false;

    void createLoadingLabel();

    float delayTimer     = 0.0f;
    ax::RefPtr<ax::Scene> nextScene;
};

#endif  // __GAME_LOADING_SCENE_H__
