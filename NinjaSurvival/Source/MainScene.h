#ifndef __MAIN_SCENE_H__
#define __MAIN_SCENE_H__

#include "axmol.h"


class MainScene : public ax::Scene
{
public:
    bool init() override;

    // touch
    void onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event);
    void onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event);

    // a selector callback
    void menuCloseCallback(ax::Object* sender);
    void menuPlayCallback(ax::Object* sender);

private:

};

#endif  // __MAIN_SCENE_H__
