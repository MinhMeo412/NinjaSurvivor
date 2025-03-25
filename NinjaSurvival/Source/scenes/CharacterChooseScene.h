#ifndef __CHARACTER_CHOOSE_SCENE_H__
#define __CHARACTER_CHOOSE_SCENE_H__

#include "axmol.h"
#include "SceneBase.h"
#include "systems/SystemManager.h"

class CharacterChooseScene : public SceneBase
{
public:
    CharacterChooseScene();

    bool init() override;
    void update(float dt) override;
private:
    ax::MenuItemImage* returnButton = nullptr;
    ax::MenuItemSprite* nextButton = nullptr;
    ax::MenuItemSprite* selectedCharacterItem = nullptr;
    std::string selectedCharacterName;

    SystemManager* systemManager = nullptr;

    // a selector callback
    void menuReturnCallback(ax::Object* sender);
    void menuNextCallback(ax::Object* sender);
    void menuUISetup();
};

#endif  // __CHARACTER_CHOOSE_SCENE_H__
