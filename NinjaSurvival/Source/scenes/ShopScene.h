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
    ax::MenuItemImage* returnButton      = nullptr;
    ax::MenuItemSprite* selectedStatItem = nullptr;
    ax::MenuItemSprite* buyButton        = nullptr;
    std::string selectedStatName;

    void menuUISetup();
    void menuReturnCallback(ax::Object* sender);
    void menuBuyCallback(ax::Object* sender);
    void setupStatButtons(ax::Node* panel, ax::Node* panelDescription, ax::Vector<ax::MenuItem*>& menuItems);
    void updateStatInfo(const std::string& name, ax::Node* panelDescription, bool isMaxLevel);
    ax::Label* createStatLabel(const std::string& name,
                                      const std::string& tag,
                                      float baseY,
                                      float xPos,
                                      Node* parent);
};

#endif  // __SHOP_SCENE_H__
