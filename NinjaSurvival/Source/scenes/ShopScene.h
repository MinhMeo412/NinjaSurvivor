#ifndef __SHOP_SCENE_H__
#define __SHOP_SCENE_H__

#include "SceneBase.h"
#include "axmol.h"

class ShopScene : public SceneBase
{
public:
    ShopScene();
    virtual bool init() override;
    virtual void update(float dt) override;



private:
    void menuUISetup();
    void menuReturnCallback(ax::Object* sender);

    ax::MenuItem* closeItem = nullptr;
    ax::MenuItemSprite* selectedStatItem;  // Khai báo biến thành viên
    std::string selectedStatName;          // Khai báo biến thành viên
    ax::MenuItemSprite* buyButton = nullptr;

};

#endif  // __SHOP_SCENE_H__
