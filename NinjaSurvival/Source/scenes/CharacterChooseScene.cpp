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
    auto background = Sprite::create("UI/background3.png");
    background->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
    this->addChild(background, 0);

    auto panelChooseCharacter = Sprite::create("UI/panelChoseCharacter2.png");
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

    auto backButton = MenuItemImage::create("UI/buttonback.png", "UI/buttonback.png",
                                            AX_CALLBACK_1(CharacterChooseScene::menuReturnCallback, this));

    float marginX = visibleSize.width * 0.05;  // 5% of screen width as margin from the left
    float marginY = visibleSize.height * 0.001;

    // Position the back button in the top-left corner
    float backX =
        origin.x + marginX + (backButton->getContentSize().width / 2);  // Center of the button, offset by margin
    float backY = origin.y + visibleSize.height - marginY - (backButton->getContentSize().height / 2);
    backButton->setPosition(Vec2(backX, backY));
    this->addChild(backButton, 6);

    auto panelDescription = Sprite::create("UI/DescriptionCharacter.png");
    // Đặt bên dưới panelChooseCharacter, cách panelChooseCharacter 2% chiều cao màn hình
    float descY = panelChooseCharacter->getPositionY() - (panelChooseCharacter->getContentSize().height / 2) -
                  (visibleSize.height * 0.01) - (panelDescription->getContentSize().height / 2);
    panelDescription->setPosition(Vec2(origin.x + visibleSize.width / 2, descY));
    this->addChild(panelDescription, 2);

    Vector<MenuItem*> menuItems;

    // Tính vị trí
    // Công thức mới: Điều chỉnh để nút nằm trong màn hình
    float panelDescRight  = panelDescription->getPositionX() + (panelDescription->getContentSize().width / 2);
    float panelDescBottom = panelDescription->getPositionY() - (panelDescription->getContentSize().height / 2);

    float playX =
        panelDescRight - (panelDescription->getContentSize().width * 0.1);  // Cách mép phải 10% chiều rộng panel
    float playY = panelDescBottom - (visibleSize.height * 0.05);            // Cách mép dưới 5% chiều cao màn hình

    // Đảm bảo nút không nằm ngoài màn hình
    playX = std::min(playX, visibleSize.width - 50.0f);  // Giả sử chiều rộng nút là 100, cách mép phải ít nhất 50
    playY = std::max(playY, 50.0f);                      // Giả sử chiều cao nút là 100, cách đáy ít nhất 50

    auto nextSprite         = Sprite::create("UI/buttonNext.png");
    auto nextSelectedSprite = Sprite::create("UI/buttonNext.png");
    auto nextButton         = MenuItemSprite::create(nextSprite, nextSelectedSprite, nullptr,
                                                     AX_CALLBACK_1(CharacterChooseScene::menuNextCallback, this));

    nextButton->setPosition(Vec2(playX, playY));
    nextButton->setScale(0.8);
    nextButton->setVisible(false);

    menuItems.pushBack(nextButton);

    // Tạo các button chọn character
    auto gameData        = GameData::getInstance();
    const auto& entities = gameData->getEntityTemplates();

    int numCharacters = 0;  // Số lượng nhân vật (có thể thay đổi)

    for (const auto& [type, templates] : entities)
    {
        if (type == "player")
        {
            numCharacters = templates.size();  // Số lượng nhân vật
            break;
        }
    }
    const int cols = 6;                                  // Số cột (6 characters per row)
    const int rows = (numCharacters + cols - 1) / cols;  // Số hàng, làm tròn lên
    float iconSpacingX =
        panelChooseCharacter->getContentSize().width * 0.08;  // Khoảng cách giữa các icon (5% chiều rộng panel)
    float iconSpacingY = panelChooseCharacter->getContentSize().height * 0.08;

    // Vị trí ban đầu
    int index = 0;

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

                // Tính tọa độ X, Y của button
                // Tính tổng chiều rộng của tất cả button và khoảng cách
                float totalButtonWidth = cols * button->getContentSize().width + (cols - 1) * iconSpacingX;
                float startX           = panelChooseCharacter->getPositionX() - (totalButtonWidth / 2) +
                               (button->getContentSize().width / 2);
                float startY = panelChooseCharacter->getPositionY() +
                               (panelChooseCharacter->getContentSize().height / 2.2) -
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
