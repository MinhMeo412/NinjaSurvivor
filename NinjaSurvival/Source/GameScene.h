#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "axmol.h"
#include "Character.h"


class GameScene : public ax::Scene
{
public:
    bool init() override;
    void update(float delta) override;

    // touch
    void onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);


    // a selector callback
    void menuCloseCallback(ax::Object* sender);
    void goToMainScene(ax::Object * sender);

private:
    ax::Layer* UILayer;
    Character* character;
};

#endif  // __GAME_SCENE_H__
