#ifndef __SCENE_BASE_H__
#define __SCENE_BASE_H__

#include "axmol.h"

class SceneBase : public ax::Scene
{
public:
    bool init() override;
    //void update(float dt) override;

protected:

    //For PC
    /*virtual void onMouseDown(ax::Event* event);
    virtual void onMouseUp(ax::Event* event);
    virtual void onMouseMove(ax::Event* event);
    virtual void onMouseScroll(ax::Event* event);
    void setupMouseListener();*/

    ax::Vec2 visibleSize;
    ax::Vec2 origin;
    ax::Rect safeArea;
    ax::Vec2 safeOrigin;
private:

    //ax::EventListenerMouse* mouseListener;  //For PC
};

#endif  //__SCENE_BASE_H__
