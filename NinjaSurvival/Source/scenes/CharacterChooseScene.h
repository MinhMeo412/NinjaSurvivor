#ifndef __CHARACTER_CHOOSE_SCENE_H__
#define __CHARACTER_CHOOSE_SCENE_H__

#include "axmol.h"
#include "SceneBase.h"

class CharacterChooseScene : public SceneBase
{
public:
    CharacterChooseScene();

    bool init() override;
    void update(float dt) override;

private:
    ax::MenuItemImage* returnButton           = nullptr;
    ax::MenuItemSprite* selectedCharacterItem = nullptr;
    std::string selectedCharacterName;

    void menuReturnCallback(ax::Object* sender);
    void menuNextCallback(ax::Object* sender);
    void menuUISetup();

    // Hàm phụ cho giao diện
    ax::Label* createStatLabel(const std::string& name,
                               const std::string& tag,
                               float baseY,
                               float xOffset,
                               float yOffset,
                               ax::Node* parent);
    void updateCharacterStats(const std::string& name, ax::Node* panelDescription, bool isAvailable);
    ax::MenuItemSprite* createBuyButton(ax::MenuItemSprite* nextButton, ax::Node* panelDescription, float x, float y);
    void setupCharacterButtons(ax::Node* panelChooseCharacter,
                               ax::Node* panelDescription,
                               ax::MenuItemSprite* nextButton,
                               ax::MenuItemSprite* buyButton,
                               ax::Vector<ax::MenuItem*>& menuItems);
};

#endif  // __CHARACTER_CHOOSE_SCENE_H__
