#ifndef __GAMESCENE_UI_LAYER_H__
#define __GAMESCENE_UI_LAYER_H__

#include "axmol.h"

class GameSceneUILayer : public ax::Layer
{
public:
    static GameSceneUILayer* create();

    bool init() override;
    void update(float dt) override;

    void increaseCoin(float coin);
    void increaseEnemyKillCount();
    void bossAlert();

    int getCollectedCoin() { return static_cast<int>(collectedCoin); }


private:
    ax::MenuItemImage* pauseButton;

    ax::Sprite* hpBarRed;
    ax::Sprite* hpBarGray;

    ax::Sprite* xpBar;
    ax::Sprite* xpBarUnder;

    float collectedCoin = 0;
    ax::Label* coinLabel = nullptr;

    ax::Label* levelLabel = nullptr;

    int enemyKillCount             = 0;
    ax::Label* enemyKillCountLabel = nullptr;

    void updateHPBar();
    void updateXPBar();
    void updateCoinLabel();
    void updateLevelLabel();
    void updateEnemyKillCountLabel();
    void gamePauseCallback(ax::Object* sender);
};

#endif  // __GAMESCENE_UI_LAYER_H__
