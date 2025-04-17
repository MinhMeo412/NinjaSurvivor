#ifndef __GAME_OVER_GAME_PAUSE_LAYER_H__
#define __GAME_OVER_GAME_PAUSE_LAYER_H__

#include "axmol.h"
#include "systems/TimeSystem.h"


class GameOverGamePauseLayer : public ax::Layer
{
public:
    static GameOverGamePauseLayer* create(bool isPlayerDead);

    GameOverGamePauseLayer(bool isPlayerDead);

    bool init() override;


private:
    TimeSystem* timeTemp;
    bool isPlayerDead;

    ax::MenuItemImage* returnButton;
    ax::MenuItemImage* quitButton;

    ax::Label* coinLabel = nullptr;
    ax::Label* enemyKillCountLabel = nullptr;
    ax::Label* timerLabel          = nullptr;

    void createUI();
    void onReturnGame(ax::Object* sender);
    void onQuitGame(ax::Object* sender);
};

#endif  // __GAME_OVER_GAME_PAUSE_LAYER_H__
