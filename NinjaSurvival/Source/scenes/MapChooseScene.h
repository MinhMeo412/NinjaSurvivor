#ifndef __MAP_CHOOSE_SCENE_H__
#define __MAP_CHOOSE_SCENE_H__

#include "axmol.h"
#include "SceneBase.h"

class MapChooseScene : public SceneBase
{
public:
    MapChooseScene();

    bool init() override;
    void update(float dt) override;

private:
    ax::MenuItemImage* closeItem        = nullptr;
    ax::MenuItemSprite* selectedMapItem = nullptr;
    std::string selectedMapName;

    void menuCloseCallback(ax::Object* sender);
    void menuPlayCallback(ax::Object* sender);
    void menuUISetup();

    ax::MenuItemSprite* createBuyButton(ax::MenuItemSprite* playButton, ax::Node* panel, float x, float y);
    void setupMapButtons(ax::Node* panel,
                         ax::MenuItemSprite* playButton,
                         ax::MenuItemSprite* buyButton,
                         ax::Vector<ax::MenuItem*>& menuItems);
    void updateMapUI(ax::MenuItemSprite* item,
                     const std::string& mapName,
                     bool isAvailable,
                     ax::MenuItemSprite* playButton,
                     ax::MenuItemSprite* buyButton);
};

#endif  // __MAP_CHOOSE_SCENE_H__
