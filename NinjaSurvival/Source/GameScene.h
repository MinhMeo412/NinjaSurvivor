#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "axmol.h"
#include "Character.h"


class GameScene : public ax::Scene
{
    enum class GameState
    {
        init = 0,
        update,
        pause,
        end,
        menu1,
        menu2,
    };
    
public:
    bool init() override;
    void update(float delta) override;

    // touch
    void onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);

    // mouse
    void onMouseDown(ax::Event* event);
    void onMouseUp(ax::Event* event);
    void onMouseMove(ax::Event* event);
    void onMouseScroll(ax::Event* event);

    // a selector callback
    void menuCloseCallback(ax::Object* sender);
    void goToMainScene(ax::Object * sender);

private:
    GameState _gameState = GameState::init;

    ax::Layer* UILayer;
    Character* character;
};

#endif  // __GAME_SCENE_H__
