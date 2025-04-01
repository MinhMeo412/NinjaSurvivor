#include "Utils.h"
#include "CharacterChooseScene.h"
#include "MapChooseScene.h"
#include "MainScene.h"
#include "systems/GameData.h"
#include <algorithm>

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
    auto background = Sprite::create("UI/background3.png");
    background->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
    this->addChild(background, 0);

    auto panelChooseCharacter = Sprite::create("UI/NeopanelChoseCharacter2.png");
    float panelY              = origin.y + visibleSize.height - (visibleSize.height * 0.05) -
                   (panelChooseCharacter->getContentSize().height / 2);
    panelChooseCharacter->setPosition(Vec2(origin.x + visibleSize.width / 2, panelY));
    this->addChild(panelChooseCharacter, 2);

    // Border
    auto drawNode = DrawNode::create();
    drawNode->setPosition(Vec2(0, 0));
    addChild(drawNode);
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4F::BLUE);

    // Menu vector chứa các menuItems

    // Return button

    Vector<MenuItem*> menuItems;
    float marginX = visibleSize.width * 0.05f;    // 5% of screen width as margin from the left
    float marginY = visibleSize.height * 0.025f;  // 0.1% of screen height as margin from the top

    // Position the back button in the top-left corner
    returnButton = Utils::createMenuItem("UI/buttonback.png", "UI/buttonback.png",
                                         AX_CALLBACK_1(CharacterChooseScene::menuReturnCallback, this),
                                         Vec2(origin.x + marginX, origin.y + visibleSize.height - marginY));
    menuItems.pushBack(returnButton);

    auto panelDescription = Sprite::create("UI/NeoDescriptionCharacter.png");
    // Đặt bên dưới panelChooseCharacter, cách panelChooseCharacter 2% chiều cao màn hình
    float descY = panelChooseCharacter->getPositionY() - (panelChooseCharacter->getContentSize().height / 2) -
                  (visibleSize.height * 0.01) - (panelDescription->getContentSize().height / 2);
    panelDescription->setPosition(Vec2(origin.x + visibleSize.width / 2, descY));
    this->addChild(panelDescription, 2);


    // Tính vị trí
    // Công thức mới: Điều chỉnh để nút nằm trong màn hình
    float panelDescRight  = panelDescription->getPositionX() + (panelDescription->getContentSize().width / 2);
    float panelDescBottom = panelDescription->getPositionY() - (panelDescription->getContentSize().height / 2);


    auto nextSprite         = Sprite::create("UI/buttonNext.png");
    auto nextSelectedSprite = Sprite::create("UI/buttonNext.png");
    auto nextButton         = MenuItemSprite::create(nextSprite, nextSelectedSprite, nullptr,
                                                     AX_CALLBACK_1(CharacterChooseScene::menuNextCallback, this));

    float playX = panelDescRight - (nextButton->getContentSize().width / 2 * 0.8) -
                  (panelDescription->getContentSize().width * 0.05);  // Cách mép phải 5% chiều rộng panel
    float playY = panelDescBottom + (nextButton->getContentSize().height / 2 * 0.8) +
                  (panelDescription->getContentSize().height * 0.05);

    nextButton->setPosition(Vec2(playX, playY));
    nextButton->setScale(0.8);
    nextButton->setVisible(false);

    menuItems.pushBack(nextButton);

    // Tạo các button chọn character
    auto gameData        = GameData::getInstance();
    const auto& entities = gameData->getEntityTemplates();

    int numCharacters = 0;  // Số lượng nhân vật
    for (const auto& [type, templates] : entities)
    {
        if (type == "player")
        {
            numCharacters = templates.size();  // Số lượng nhân vật
            break;
        }
    }

    const int cols = 2;  // Số cột (có thể thay đổi nếu muốn)
    const int rows = (numCharacters + cols - 1) / cols;
      // Số hàng, làm tròn lên
    // Khoảng cách giữa các button
    float iconSpacingX = panelChooseCharacter->getContentSize().width * 0.08;  // 5% chiều rộng panel
    float iconSpacingY = panelChooseCharacter->getContentSize().height * 0.08;
    
    // Vị trí ban đầu
    int index = 0;

    // Tạo các button trước để lấy kích thước mẫu (giả sử tất cả button có cùng kích thước)
    std::vector<MenuItem*> buttons;  // Lưu tạm các button để tính toán
    for (const auto& [type, templates] : entities)
    {
        if (type == "player")
        {
            for (const auto& [name, templ] : templates)
            {
                std::string spritePath = templ.sprite ? templ.profilePhoto.value() : "CloseNormal.png";
                bool available         = templ.available;
                auto button = Utils::createCharacterButton(spritePath, name, available, selectedCharacterName,
                                                           selectedCharacterItem, nextButton);

                // Tính vị trí button trong lưới
                int row = index / cols;  // Hàng
                int col = index % cols;  // Cột

                float totalButtonWidth = cols * button->getContentSize().width + (cols - 1) * iconSpacingX;
                float startX = panelChooseCharacter->getPositionX() - (totalButtonWidth / 2) + (button->getContentSize().width / 2);
                float startY = panelChooseCharacter->getPositionY() +
                               (panelChooseCharacter->getContentSize().height / 2.5) +
                               (button->getContentSize().height / 2) - iconSpacingY;

                float buttonX = startX + col * (button->getContentSize().width + iconSpacingX);
                float buttonY = startY - row * (button->getContentSize().height + iconSpacingY);

                button->setPosition(Vec2(buttonX, buttonY));
                //                button->setScale(2);  // Có thể điều chỉnh kích thước button
                menuItems.pushBack(button);
                index++;
            }
        }
    }

    // Tạo Menu chứa tất cả button
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 6);
}
