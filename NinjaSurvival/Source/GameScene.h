#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "axmol.h"
#include "SceneBase.h"
#include "Character.h"
#include "Joystick.h"

class GameScene : public SceneBase
{
public:
    bool init() override;
    void update(float delta) override;

    void menuCloseCallback(ax::Object* sender);
    void goToMainScene(ax::Object * sender);

private:
    ax::Layer* UILayer;
    Character* character;
    Joystick* joystick;
    ax::Camera* camera;
};

#endif  // __GAME_SCENE_H__
