#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "axmol.h"
#include "SceneBase.h"

class MainScene : public SceneBase
{
public:
    MainScene();

    bool init() override;
    void update(float dt) override;

    // a selector callback
    void menuCloseCallback(ax::Object* sender);
    void menuPlayCallback(ax::Object* sender);

private:
    ax::MenuItemImage* playItem;
    // ax::MenuItemImage* scoreItem;
    ax::MenuItemImage* closeItem;
};

#endif  // __MAIN_SCENE_H__
