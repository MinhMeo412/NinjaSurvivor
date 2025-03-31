#include "Utils.h"
#include "CharacterChooseScene.h"
#include "MapChooseScene.h"
#include "MainScene.h"
#include "systems/GameData.h"


using namespace ax;

CharacterChooseScene::CharacterChooseScene() : SceneBase("CharacterChooseScene") {}

bool CharacterChooseScene::init()
{
    if (!SceneBase::init())
    {
        return false;
    }

    systemManager = SystemManager::getInstance();
    systemManager->resetSystems();
        
    menuUISetup();

    return true;
}

void CharacterChooseScene::update(float dt) {}

void CharacterChooseScene::menuReturnCallback(ax::Object* sender)
{
    auto scene = utils::createInstance<MainScene>();
    _director->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}

void CharacterChooseScene::menuNextCallback(ax::Object* sender)
{
    //Kiểm tra
    if (selectedCharacterName.empty())
    {
        AXLOG("Error: No character selected");
        return;
    }

    //Truyền tên nhân vật được chọn cho GameData lưu trữ
    GameData::getInstance()->setSelectedCharacter(selectedCharacterName);

    //Đổi scene
    auto scene = utils::createInstance<MapChooseScene>();
    _director->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}

void CharacterChooseScene::menuUISetup()
{
    // Border
    auto drawNode = DrawNode::create();
    drawNode->setPosition(Vec2(0, 0));
    addChild(drawNode);
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4F::BLUE);

    //Menu vector chứa các menuItems
    Vector<MenuItem*> menuItems;

    // Return button
    returnButton =
        Utils::createMenuItem("CloseNormal.png", "CloseSelected.png",
                                        AX_CALLBACK_1(CharacterChooseScene::menuReturnCallback, this), Vec2(1100, 100));
    auto label2 = Label::createWithTTF("Exit button", "fonts/Marker Felt.ttf", 24);
    label2->setPosition(returnButton->getPosition() + Vec2(100, 0));
    this->addChild(label2, 1);
    menuItems.pushBack(returnButton);

    // Next button
    auto nextSprite = Sprite::create("CloseNormal.png");
    auto nextSelectedSprite = Sprite::create("CloseSelected.png");
    nextButton              = MenuItemSprite::create(nextSprite, nextSelectedSprite, nullptr,
                                                     AX_CALLBACK_1(CharacterChooseScene::menuNextCallback, this));
    nextButton->setPosition(Vec2(Director::getInstance()->getVisibleSize().width / 2, 100));
    nextButton->setVisible(false);
    menuItems.pushBack(nextButton);

    

    // Tạo các button chọn character
    auto gameData    = GameData::getInstance();
    const auto& entities = gameData->getEntityTemplates();

    float xPos = 200;
    for (const auto& [type, templates] : entities)
    {
        if (type == "player")
        {
            for (const auto& [name, templ] : templates)
            {
                std::string spritePath =
                    templ.profilePhoto.has_value() ? templ.profilePhoto.value() : "CloseNormal.png";
                bool available         = templ.available;
                auto button =
                    Utils::createCharacterButton(spritePath,             
                                                 name,                 
                                                 available,              
                                                 selectedCharacterName,  
                                                 selectedCharacterItem,  
                                                 nextButton);
                button->setPosition(Vec2(xPos, 200));
                menuItems.pushBack(button);
                xPos += 200;
            }
        }
    }

    // Tạo Menu chứa tất cả button
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu);
}
