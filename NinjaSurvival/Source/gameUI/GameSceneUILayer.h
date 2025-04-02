#ifndef __GAMESCENE_UILAYER_H__
#define __GAMESCENE_UILAYER_H__

#include "axmol.h"

class GameSceneUILayer : public ax::Layer
{
public:
    static GameSceneUILayer* create();

    bool init() override;
    void update(float dt) override;

    void increaseCoin(float coin);
private:
    ax::MenuItemImage* pauseButton;

    ax::Sprite* hpBarRed;
    ax::Sprite* hpBarGray;

    ax::Sprite* xpBar;
    ax::Sprite* xpBarUnder;

    ax::Label* coinLabel = nullptr;
    float collectedCoin;

    void updateHPBar();
    void updateXPBar();
    void updateCoinLabel();
    void gamePauseCallback(ax::Object* sender);
};

#endif  // __GAMESCENE_UILAYER_H__
