#ifndef __SCENE_BASE_H__
#define __SCENE_BASE_H__

#include "axmol.h"

class SceneBase : public ax::Scene
{
public:
    bool init() override;
    void update(float dt) override;

    SceneBase(const std::string& sceneName = "SceneBase");
    virtual ~SceneBase();

protected:
    ax::Vec2 visibleSize;
    ax::Vec2 origin;
    ax::Rect safeArea;
    ax::Vec2 safeOrigin;

    int sceneID = 0;
    static int s_sceneID;
    std::string sceneName;

private:
};

#endif  //__SCENE_BASE_H__
