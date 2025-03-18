#ifndef __GAMESCENE_UILAYER_H__
#define __GAMESCENE_UILAYER_H__

#include "axmol.h"

class GameSceneUILayer : public ax::Layer
{
public:
    static GameSceneUILayer* create();

    bool init() override;
    void update(float dt) override;

private:
    ax::MenuItemImage* closeButton;


    void menuCloseCallback(ax::Object* sender);
};

#endif  // __GAMESCENE_UILAYER_H__
