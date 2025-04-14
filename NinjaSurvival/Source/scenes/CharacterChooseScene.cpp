#include "CharacterChooseScene.h"
#include "Utils.h"
#include "MapChooseScene.h"
#include "MainScene.h"
#include "systems/GameData.h"
#include "systems/ShopSystem.h"
#include "AudioManager.h"

using namespace ax;

CharacterChooseScene::CharacterChooseScene() : SceneBase("CharacterChooseScene") {}

bool CharacterChooseScene::init()
{
    if (!SceneBase::init())
    {
        return false;
    }

    // Thêm: Đồng bộ để đảm bảo chỉ số mới nhất
    GameData::getInstance()->syncStatsWithShopSystem();

    menuUISetup();
    return true;
}

void CharacterChooseScene::update(float dt)
{
    // Dự phòng cho cập nhật sau
}

void CharacterChooseScene::menuReturnCallback(Object* sender)
{
    // âm thanh click
    AudioManager::getInstance()->playSound("button_click", false, 1.0f, "click");
    AudioManager::getInstance()->stopSound("background");

    _director->replaceScene(TransitionFade::create(TRANSITION_TIME, utils::createInstance<MainScene>()));
}

void CharacterChooseScene::menuNextCallback(Object* sender)
{
    if (selectedCharacterName.empty())
    {
        AXLOG("Lỗi: Chưa chọn nhân vật nào");
        return;
    }
    _director->getEventDispatcher()->setEnabled(false);
    GameData::getInstance()->setSelectedCharacter(selectedCharacterName);

    // âm thanh click
    AudioManager::getInstance()->playSound("button_click", false, 1.0f, "click");

    _director->replaceScene(TransitionFade::create(TRANSITION_TIME, utils::createInstance<MapChooseScene>()));
    this->scheduleOnce([this](float) { _director->getEventDispatcher()->setEnabled(true); }, TRANSITION_TIME,
                       "enableInput");
}

void CharacterChooseScene::menuUISetup()
{
    float marginX = visibleSize.width * 0.05f;
    float marginY = visibleSize.height * 0.025f;

    // Tạo background
    auto background = Sprite::create("UI/background3.png");
    if (!background)
    {
        AXLOG("Lỗi: Không thể tạo background3.png");
        return;
    }
    background->setPosition(origin + visibleSize / 2);
    this->addChild(background, 0);

    // Tạo panel chọn nhân vật
    auto panelChooseCharacter = Sprite::create("UI/NeopanelChoseCharacter2.png");
    if (!panelChooseCharacter)
    {
        AXLOG("Lỗi: Không thể tạo NeopanelChoseCharacter2.png");
        return;
    }
    float panelY = origin.y + visibleSize.height * 0.95f - panelChooseCharacter->getContentSize().height / 2;
    panelChooseCharacter->setPosition(origin.x + visibleSize.width / 2, panelY);
    this->addChild(panelChooseCharacter, 2);

    // Vẽ viền debug
    auto drawNode = DrawNode::create();
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4F::BLUE);
    this->addChild(drawNode, 0);

    // Hiển thị coin
    auto shopData = ShopSystem::getInstance();
    auto coinLabel =
        Label::createWithTTF(StringUtils::format("%d", shopData->getCoins()), "fonts/Pixelpurl-0vBPP.ttf", 24);
    if (!coinLabel)
    {
        AXLOG("Lỗi: Không thể tạo coinLabel");
        return;
    }
    auto coinSprite = Sprite::create("coin.png");
    if (!coinSprite)
    {
        AXLOG("Lỗi: Không thể tạo coinSprite");
        return;
    }
    float coinLabelWidth  = coinLabel->getContentSize().width;
    float coinSpriteWidth = coinSprite->getContentSize().width;
    float totalWidth      = coinLabelWidth + coinSpriteWidth + 10.0f;
    float startPosX       = origin.x + visibleSize.width - marginX - totalWidth;
    float posY            = origin.y + visibleSize.height - marginY;
    coinLabel->setPosition(startPosX + coinLabelWidth / 2, posY);
    coinLabel->setAlignment(TextHAlignment::LEFT);
    this->addChild(coinLabel, 5, "coinLabel");
    coinSprite->setPosition(startPosX + coinLabelWidth + 10.0f + coinSpriteWidth / 2, posY);
    coinSprite->setScale(3);
    this->addChild(coinSprite, 5, "coinSprite");

    // Khởi tạo menu items
    Vector<MenuItem*> menuItems;

    // Tạo nút Return
    returnButton = Utils::createMenuItem("UI/buttonback.png", "UI/buttonback.png",
                                         AX_CALLBACK_1(CharacterChooseScene::menuReturnCallback, this),
                                         Vec2(origin.x + marginX, origin.y + visibleSize.height - marginY));
    if (!returnButton)
    {
        AXLOG("Lỗi: Không thể tạo nút Return");
        return;
    }
    menuItems.pushBack(returnButton);

    // Tạo panel mô tả
    auto panelDescription = Sprite::create("UI/NeoDescriptionCharacter.png");
    if (!panelDescription)
    {
        AXLOG("Lỗi: Không thể tạo NeoDescriptionCharacter.png");
        return;
    }
    float descY = panelChooseCharacter->getPositionY() - panelChooseCharacter->getContentSize().height / 2 -
                  visibleSize.height * 0.01f - panelDescription->getContentSize().height / 2;
    panelDescription->setPosition(origin.x + visibleSize.width / 2, descY);
    this->addChild(panelDescription, 2);

    // Tính vị trí nút Next/Buy
    auto nextSprite = Sprite::create("UI/buttonNext.png");
    if (!nextSprite)
    {
        AXLOG("Lỗi: Không thể tạo buttonNext.png");
        return;
    }
    float panelDescRight  = panelDescription->getPositionX() + panelDescription->getContentSize().width / 2;
    float panelDescBottom = panelDescription->getPositionY() - panelDescription->getContentSize().height / 2;
    float playX           = panelDescRight - (nextSprite->getContentSize().width / 2 * 0.8f) -
                  panelDescription->getContentSize().width * 0.045f;
    float playY = panelDescBottom + (nextSprite->getContentSize().height / 2 * 0.8f) +
                  panelDescription->getContentSize().height * 0.075f;

    // Tạo nút Next
    auto nextButton =
        Utils::createButton("UI/buttonNext.png", AX_CALLBACK_1(CharacterChooseScene::menuNextCallback, this),
                            Vec2(playX, playY), 0.8f, false);
    if (!nextButton)
    {
        AXLOG("Lỗi: Không thể tạo nút Next");
        return;
    }
    menuItems.pushBack(nextButton);

    // Tạo nút Buy
    auto buyButton = createBuyButton(dynamic_cast<MenuItemSprite*>(nextButton), panelDescription, playX, playY);
    if (!buyButton)
    {
        AXLOG("Lỗi: Không thể tạo nút Buy");
        return;
    }
    menuItems.pushBack(buyButton);

    // Tạo button nhân vật
    setupCharacterButtons(panelChooseCharacter, panelDescription, dynamic_cast<MenuItemSprite*>(nextButton), buyButton,
                          menuItems);

    // Tạo menu
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 6);
}



void CharacterChooseScene::updateCharacterStats(const std::string& name, Node* panelDescription, bool isAvailable)
{
    auto gameData       = GameData::getInstance();
    auto shopData       = ShopSystem::getInstance();
    auto characterLabel = dynamic_cast<Label*>(panelDescription->getChildByName("label" + name));
    auto healthLabel    = dynamic_cast<Label*>(panelDescription->getChildByName("healthLabel" + name));
    auto weaponLabel    = dynamic_cast<Label*>(panelDescription->getChildByName("weaponLabel" + name));
    auto attackLabel    = dynamic_cast<Label*>(panelDescription->getChildByName("attackLabel" + name));
    auto speedLabel     = dynamic_cast<Label*>(panelDescription->getChildByName("speedLabel" + name));
    auto unlockLabel    = dynamic_cast<Label*>(panelDescription->getChildByName("unlockLabel" + name));

    if (!characterLabel || !healthLabel || !weaponLabel || !attackLabel || !speedLabel || !unlockLabel)
    {
        AXLOG("Lỗi: Thiếu label cho nhân vật %s", name.c_str());
        return;
    }

    // Ẩn tất cả nhãn
    for (auto* child : panelDescription->getChildren())
        if (dynamic_cast<Label*>(child))
            child->setVisible(false);

    characterLabel->setVisible(true);

    if (!isAvailable)
    {
        unlockLabel->setString(StringUtils::format("Unlock: %d", shopData->getCost("entities", name)));
        unlockLabel->setVisible(true);
        return;
    }

    const auto& entities = gameData->getEntityTemplates();
    if (entities.find("player") == entities.end() || entities.at("player").find(name) == entities.at("player").end())
    {
        AXLOG("Lỗi: Không tìm thấy nhân vật %s trong entityTemplates", name.c_str());
        return;
    }

    auto templ = entities.at("player").at(name);
    // Sửa: Dùng %.0f để hiển thị số nguyên
    healthLabel->setString(templ.health.has_value() ? StringUtils::format("Health: %.0f", templ.health->maxHealth)
                                                    : "Health: N/A");
    attackLabel->setString(templ.attack.has_value() ? StringUtils::format("Attack: %.0f", templ.attack->baseDamage)
                                                    : "Attack: N/A");
    speedLabel->setString(templ.speed.has_value() ? StringUtils::format("Speed: %.0f", templ.speed->speed)
                                                  : "Speed: N/A");
    weaponLabel->setString(
        templ.weaponInventory.has_value() && !templ.weaponInventory->weapons.empty() &&
                !templ.weaponInventory->weapons[0].first.empty()
            ? StringUtils::format("Default Weapon: %s", templ.weaponInventory->weapons[0].first.c_str())
            : "Default Weapon: None");

    healthLabel->setVisible(true);
    attackLabel->setVisible(true);
    speedLabel->setVisible(true);
    weaponLabel->setVisible(true);
}

MenuItemSprite* CharacterChooseScene::createBuyButton(MenuItemSprite* nextButton,
                                                      Node* panelDescription,
                                                      float x,
                                                      float y)
{
    return dynamic_cast<MenuItemSprite*>(
        Utils::createButton("UI/buttonBuy.png", [this, nextButton, panelDescription](Object* sender) {
        auto shopData    = ShopSystem::getInstance();
        auto gameData    = GameData::getInstance();
        int currentCoins = shopData->getCoins();
        int cost         = shopData->getCost("entities", selectedCharacterName);

        if (selectedCharacterName.empty() || currentCoins < cost)
        {
            auto errorLabel = Label::createWithTTF("Không đủ coin!", "fonts/Pixelpurl-0vBPP.ttf", 20);
            if (errorLabel)
            {
                errorLabel->setPosition(visibleSize / 2);
                this->addChild(errorLabel, 20);
                errorLabel->runAction(Sequence::create(FadeOut::create(2.0f), RemoveSelf::create(), nullptr));
            }
            dynamic_cast<Node*>(sender)->setVisible(false);
            return;
        }

        shopData->setCoins(currentCoins - cost);
        shopData->setAvailable("entities", selectedCharacterName, true);
        gameData->setCharacterAvailable(selectedCharacterName, true);
        shopData->syncCharactersWithGameData();

        if (selectedCharacterItem)
        {
            auto& entities =
                const_cast<std::unordered_map<std::string, std::unordered_map<std::string, EntityTemplate>>&>(
                    gameData->getEntityTemplates());
            if (entities.find("player") != entities.end() &&
                entities["player"].find(selectedCharacterName) != entities["player"].end())
            {
                entities["player"][selectedCharacterName].available = true;
            }

            Utils::updateItemUI(selectedCharacterItem, panelDescription, true);
            dynamic_cast<Node*>(sender)->setVisible(false);
            nextButton->setVisible(true);
            updateCharacterStats(selectedCharacterName, panelDescription, true);
        }

        if (auto* coinLabel = dynamic_cast<Label*>(this->getChildByName("coinLabel")))
            coinLabel->setString(StringUtils::format("Coins: %d", shopData->getCoins()));

        shopData->saveToFile(FileUtils::getInstance()->getWritablePath() + "savegame.json");
        AXLOG("Đã mở khóa nhân vật: %s", selectedCharacterName.c_str());
    }, Vec2(x, y), 0.8f, false));
}

Label* CharacterChooseScene::createStatLabel(const std::string& name,
                                             const std::string& tag,
                                             float baseY,
                                             float xOffset,
                                             float yOffset,
                                             Node* parent)
{
    auto label = Label::createWithTTF("", "fonts/Pixelpurl-0vBPP.ttf", 18);
    if (!label)
    {
        AXLOG("Lỗi: Không thể tạo label %s", tag.c_str());
        return nullptr;
    }
    label->setPosition(parent->getContentSize().width * 0.18f + xOffset, baseY - yOffset);
    label->setAlignment(TextHAlignment::LEFT);
    label->setVisible(false);
    parent->addChild(label, 6, tag + name);
    return label;
}

void CharacterChooseScene::setupCharacterButtons(Node* panelChooseCharacter,
                                                 Node* panelDescription,
                                                 MenuItemSprite* nextButton,
                                                 MenuItemSprite* buyButton,
                                                 Vector<MenuItem*>& menuItems)
{
    auto gameData        = GameData::getInstance();
    const auto& entities = gameData->getEntityTemplates();
    if (entities.find("player") == entities.end())
    {
        AXLOG("Lỗi: Không có nhân vật loại 'player' trong entityTemplates");
        return;
    }

    int numCharacters = entities.at("player").size();
    const int cols    = 2;
    const int rows    = (numCharacters + cols - 1) / cols;

    float iconSpacingX = panelChooseCharacter->getContentSize().width * 0.06f;
    float iconSpacingY = panelChooseCharacter->getContentSize().height * 0.03f;
    float panelLeft    = panelChooseCharacter->getPositionX() - panelChooseCharacter->getContentSize().width / 2.4f;
    float startY =
        panelChooseCharacter->getPositionY() + panelChooseCharacter->getContentSize().height / 2.5f - iconSpacingY;
    float startX = panelLeft + iconSpacingX;

    int index = 0;
    for (const auto& [name, templ] : entities.at("player"))
    {
        auto characterLabel = Label::createWithTTF(name, "fonts/Pixelpurl-0vBPP.ttf", 30);
        if (!characterLabel)
        {
            AXLOG("Lỗi: Không thể tạo label cho nhân vật %s", name.c_str());
            continue;
        }
        characterLabel->setPosition(panelDescription->getContentSize().width / 2,
                                    panelDescription->getContentSize().height * 0.85f);
        characterLabel->setVisible(false);
        panelDescription->addChild(characterLabel, 6, "label" + name);

        float characterLabelBottomY =
            panelDescription->getContentSize().height * 0.85f - characterLabel->getContentSize().height * 0.5f;
        float baseY = characterLabelBottomY - 20.0f;

        auto healthLabel = createStatLabel(name, "healthLabel", baseY, 0.0f, 0.0f, panelDescription);
        auto weaponLabel = createStatLabel(name, "weaponLabel", baseY, 150.0f, 0.0f, panelDescription);
        auto attackLabel = createStatLabel(name, "attackLabel", baseY, 0.0f,
                                           20.0f, panelDescription);
        auto speedLabel  = createStatLabel(name, "speedLabel", baseY, 0.0f,
                                           30.0f, panelDescription);
        auto unlockLabel = createStatLabel(name, "unlockLabel", baseY, 0.0f, 0.0f, panelDescription);

        if (!healthLabel || !weaponLabel || !attackLabel || !speedLabel || !unlockLabel)
        {
            AXLOG("Lỗi: Không thể tạo đầy đủ label cho nhân vật %s", name.c_str());
            continue;
        }

        auto button = Utils::createCharacterButton(
            templ.profilePhoto.has_value() ? templ.profilePhoto.value() : "CloseNormal.png", name, templ.available,
            selectedCharacterName, selectedCharacterItem, nextButton);
        if (!button)
        {
            AXLOG("Lỗi: Không thể tạo button cho nhân vật %s", name.c_str());
            continue;
        }

        button->setCallback([this, name, buyButton, nextButton, panelDescription](Object* sender) {
            selectedCharacterName = name;

            if (selectedCharacterItem && selectedCharacterItem != sender)
            {
                if (auto* oldBorder = selectedCharacterItem->getChildByName("border"))
                    oldBorder->setVisible(false);
            }
            if (selectedCharacterItem)
                selectedCharacterItem->setEnabled(true);
            selectedCharacterItem = dynamic_cast<MenuItemSprite*>(sender);
            selectedCharacterItem->setEnabled(false);
            if (auto* border = selectedCharacterItem->getChildByName("border"))
                border->setVisible(true);

            bool isAvailable     = false;
            const auto& entities = GameData::getInstance()->getEntityTemplates();
            if (entities.find("player") != entities.end() &&
                entities.at("player").find(name) != entities.at("player").end())
            {
                isAvailable = entities.at("player").at(name).available;
            }
            else
            {
                AXLOG("Lỗi: Không tìm thấy nhân vật %s trong entityTemplates", name.c_str());
            }

            updateCharacterStats(name, panelDescription, isAvailable);
            Utils::updateItemUI(selectedCharacterItem, panelDescription, isAvailable);
            buyButton->setVisible(!isAvailable);
            nextButton->setVisible(isAvailable);
        });

        button->setUserData(characterLabel);
        button->setPosition(Vec2(startX + index * (button->getContentSize().width + iconSpacingX), startY));
        menuItems.pushBack(button);
        index++;
    }
}

