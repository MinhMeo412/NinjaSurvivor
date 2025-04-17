#include "ShopScene.h"
#include "Utils.h"
#include "MainScene.h"
#include "systems/ShopSystem.h"
#include "systems/GameData.h"
#include "AudioManager.h"

using namespace ax;

ShopScene::ShopScene() : SceneBase("ShopScene"), selectedStatItem(nullptr), selectedStatName("") {}

bool ShopScene::init()
{
    if (!SceneBase::init())
    {
        return false;
    }

    auto shopData = ShopSystem::getInstance();
    if (!shopData->loadSaveGame())
    {
        AXLOG("Không thể tải savegame.json trong ShopScene::init()");
    }
    else
    {
        AXLOG("Đã tải thành công savegame.json trong ShopScene::init()");
    }

    menuUISetup();
    return true;
}

void ShopScene::update(float dt)
{
    // Dự phòng cho cập nhật sau
}

void ShopScene::menuUISetup()
{
    // Sử dụng safeArea cho vị trí UI
    const auto& safeOrigin = safeArea.origin;
    const auto& safeSize   = safeArea.size;

    float marginX = safeSize.width * 0.05f;
    float marginY = safeSize.height * 0.025f;

    // Tạo background (phủ toàn màn hình)
    auto background = Sprite::create("UI/background3.png");
    if (!background)
    {
        AXLOG("Lỗi: Không thể tạo background3.png");
        return;
    }
    background->setPosition(origin + visibleSize / 2);
    this->addChild(background, 0);

    // Tạo panel chính
    auto panel = Sprite::create("UI/panelShop.png");
    if (!panel)
    {
        AXLOG("Lỗi: Không thể tạo panelShop.png");
        return;
    }
    float panelY = safeOrigin.y + safeSize.height * 0.95f - panel->getContentSize().height / 2;
    panel->setPosition(safeOrigin.x + safeSize.width / 2, panelY);
    this->addChild(panel, 2);

    // Tạo panel mô tả
    auto panelDescription = Sprite::create("UI/NeoPanelDecripsionInSkillUp2.png");
    if (!panelDescription)
    {
        AXLOG("Lỗi: Không thể tạo NeoPanelDecripsionInSkillUp2.png");
        return;
    }
    float descY = panelY - panel->getContentSize().height / 2 - safeSize.height * 0.01f -
                  panelDescription->getContentSize().height / 2;
    panelDescription->setPosition(safeOrigin.x + safeSize.width / 2, descY);
    this->addChild(panelDescription, 3, "panelDescription");

    // Hiển thị coin
    auto shopData = ShopSystem::getInstance();
    auto coinLabel =
        Label::createWithTTF(StringUtils::format("%d", shopData->getCoins()), "fonts/Pixelpurl-0vBPP.ttf", 40);
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
    float startPosX       = safeOrigin.x + safeSize.width - marginX - totalWidth;
    float posY            = safeOrigin.y + safeSize.height - marginY;
    coinLabel->setPosition(startPosX + coinLabelWidth / 2, posY);
    coinLabel->setAlignment(TextHAlignment::LEFT);
    this->addChild(coinLabel, 5, "coinLabel");
    coinSprite->setPosition(startPosX + coinLabelWidth + 10.0f + coinSpriteWidth / 2, posY);
    coinSprite->setScale(3);
    this->addChild(coinSprite, 5, "coinSprite");

    // Vẽ viền debug cho safeArea
    auto drawNode = DrawNode::create();
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4F::BLUE);
    this->addChild(drawNode, 0);

    // Khởi tạo menu items
    Vector<MenuItem*> menuItems;

    // Tạo nút Return
    returnButton = Utils::createMenuItem("UI/buttonback.png", "UI/buttonback.png",
                                         AX_CALLBACK_1(ShopScene::menuReturnCallback, this),
                                         Vec2(safeOrigin.x + marginX, safeOrigin.y + safeSize.height - marginY));
    if (!returnButton)
    {
        AXLOG("Lỗi: Không thể tạo nút Return");
        return;
    }
    menuItems.pushBack(returnButton);

    // Tạo nút Buy
    auto buySprite = Sprite::create("UI/buttonBuy.png");
    if (!buySprite)
    {
        AXLOG("Lỗi: Không thể tạo buttonBuy.png");
        return;
    }
    float buyX = panelDescription->getPositionX() + panelDescription->getContentSize().width / 2 -
                 buySprite->getContentSize().width * 0.4f - panelDescription->getContentSize().width * 0.05f;
    float buyY = panelDescription->getPositionY() - panelDescription->getContentSize().height * 0.25f +
                 buySprite->getContentSize().height * 0.3f;
    buyButton = dynamic_cast<MenuItemSprite*>(Utils::createButton(
        "UI/buttonBuy.png", AX_CALLBACK_1(ShopScene::menuBuyCallback, this), Vec2(buyX, buyY), 0.8f, false));
    if (!buyButton)
    {
        AXLOG("Lỗi: Không thể tạo nút Buy");
        return;
    }
    buyButton->setVisible(false);
    menuItems.pushBack(buyButton);

    // Tạo các nút stat
    setupStatButtons(panel, panelDescription, menuItems);

    // Tạo menu
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 10);
}

void ShopScene::menuReturnCallback(Object* sender)
{
    // âm thanh click
    AudioManager::getInstance()->playSound("button_click", false, 1.0f, "click");
    AudioManager::getInstance()->stopSound("background");

    _director->replaceScene(TransitionFade::create(TRANSITION_TIME, utils::createInstance<MainScene>()));
}

void ShopScene::menuBuyCallback(Object* sender)
{
    if (selectedStatName.empty())
    {
        AXLOG("Lỗi: Chưa chọn stat nào để nâng cấp");
        return;
    }

    auto shopData    = ShopSystem::getInstance();
    int currentCoins = shopData->getCoins();
    int cost         = shopData->getCost("Stat", selectedStatName);

    if (currentCoins < cost)
    {
        auto errorLabel = Label::createWithTTF("Không đủ coin!", "fonts/Pixelpurl-0vBPP.ttf", 20);
        if (errorLabel)
        {
            errorLabel->setPosition(visibleSize / 2);
            this->addChild(errorLabel, 20);
            errorLabel->runAction(Sequence::create(FadeOut::create(2.0f), RemoveSelf::create(), nullptr));
        }
        return;
    }

    if (shopData->upgradeStat(selectedStatName))
    {
        shopData->setCoins(currentCoins - cost);
        int newLevel      = shopData->getLevel("Stat", selectedStatName);
        int newLevelValue = shopData->getStatLevelValue("Stat", selectedStatName);
        int newCost       = shopData->getCost("Stat", selectedStatName);
        bool isMaxLevel   = newLevel >= shopData->getLevelLimit("Stat", selectedStatName);

        // Cập nhật giao diện
        if (selectedStatItem)
        {
            if (auto* label = dynamic_cast<Label*>(selectedStatItem->getChildByName("label")))
            {
                label->setString(
                    StringUtils::format("Lv.%s", isMaxLevel ? "Max" : StringUtils::format("%d", newLevel).c_str()));
            }
        }

        updateStatInfo(selectedStatName, getChildByName("panelDescription"), isMaxLevel);
        buyButton->setVisible(!isMaxLevel);

        if (auto* coinLabel = dynamic_cast<Label*>(getChildByName("coinLabel")))
        {
            coinLabel->setString(StringUtils::format("%d", shopData->getCoins()));
        }

        // Hiển thị thông báo thành công
        auto successLabel = Label::createWithTTF(StringUtils::format("Đã nâng cấp %s!", selectedStatName.c_str()),
                                                 "fonts/Pixelpurl-0vBPP.ttf", 20);
        if (successLabel)
        {
            successLabel->setPosition(visibleSize / 2);
            this->addChild(successLabel, 20);
            successLabel->runAction(Sequence::create(FadeOut::create(2.0f), RemoveSelf::create(), nullptr));
        }

        shopData->saveToFile(FileUtils::getInstance()->getWritablePath() + "savegame.json");
        AXLOG("Đã nâng cấp stat: %s, level=%d, levelValue=%d, cost=%d", selectedStatName.c_str(), newLevel,
              newLevelValue, newCost);
    }
    else
    {
        AXLOG("Lỗi: Nâng cấp stat %s thất bại", selectedStatName.c_str());
    }
}

void ShopScene::setupStatButtons(Node* panel, Node* panelDescription, Vector<MenuItem*>& menuItems)
{
    auto shopData         = ShopSystem::getInstance();
    const auto& shopItems = shopData->getShopData();
    int index             = 0;
    const int cols        = 6;
    float iconSpacingX    = panel->getContentSize().width * 0.12f;
    float startX          = panel->getPositionX() - panel->getContentSize().width * 0.35f;
    float startY          = panel->getPositionY() + panel->getContentSize().height * 0.39f;

    // Lề trái cố định cho các label
    float leftMargin = panelDescription->getContentSize().width * 0.1f;

    for (const auto& item : shopItems)
    {
        if (item.type != "Stat")
            continue;

        // Tạo các label riêng cho thông tin stat
        float baseY        = panelDescription->getContentSize().height * 0.8f;
        auto nameLabel     = createStatLabel(item.name, "nameLabel", baseY, leftMargin, panelDescription);
        auto levelLabel    = createStatLabel(item.name, "levelLabel", baseY - 30.0f, leftMargin, panelDescription);
        auto increaseLabel = createStatLabel(item.name, "increaseLabel", baseY - 60.0f, leftMargin, panelDescription);
        auto costLabel     = createStatLabel(item.name, "costLabel", baseY - 90.0f, leftMargin, panelDescription);

        if (!nameLabel || !levelLabel || !increaseLabel || !costLabel)
        {
            AXLOG("Lỗi: Không thể tạo đầy đủ label cho stat %s", item.name.c_str());
            continue;
        }

        // Cập nhật thông tin stat
        bool isMaxLevel =
            item.level.has_value() && item.levelLimit.has_value() && item.level.value() >= item.levelLimit.value();
        updateStatInfo(item.name, panelDescription, isMaxLevel);

        // Tạo nút stat
        std::string spritePath = item.name + ".png";
        std::string statValue  = StringUtils::format(
            "Lv.%s", isMaxLevel ? "Max" : StringUtils::format("%d", item.level.value_or(0)).c_str());
        auto statButton = Utils::createStatButton(spritePath, statValue);
        if (!statButton)
        {
            AXLOG("Lỗi: Không thể tạo statButton cho %s", item.name.c_str());
            continue;
        }

        statButton->setScale(1.5f);
        float buttonX =
            startX + (statButton->getContentSize().width * 1.5f + iconSpacingX) * index;  // Adjusted for scale
        statButton->setPosition(buttonX, startY);

        // Callback chọn stat
        statButton->setCallback([this, name = item.name, panelDescription, shopData](Object* sender) {
            if (selectedStatItem && selectedStatItem != sender)
            {
                if (auto* oldLabel = dynamic_cast<Label*>(selectedStatItem->getChildByName("label")))
                    oldLabel->setVisible(false);
            }

            selectedStatName = name;
            selectedStatItem = dynamic_cast<MenuItemSprite*>(sender);

            // Ẩn tất cả label
            for (auto* child : panelDescription->getChildren())
                if (dynamic_cast<Label*>(child))
                    child->setVisible(false);

            // Hiển thị các label của stat được chọn
            if (auto* label = dynamic_cast<Label*>(panelDescription->getChildByName("nameLabel" + name)))
                label->setVisible(true);
            if (auto* label = dynamic_cast<Label*>(panelDescription->getChildByName("levelLabel" + name)))
                label->setVisible(true);
            if (auto* label = dynamic_cast<Label*>(panelDescription->getChildByName("increaseLabel" + name)))
                label->setVisible(true);
            if (auto* label = dynamic_cast<Label*>(panelDescription->getChildByName("costLabel" + name)))
                label->setVisible(true);

            if (selectedStatItem)
            {
                if (auto* buttonLabel = dynamic_cast<Label*>(selectedStatItem->getChildByName("label")))
                    buttonLabel->setVisible(true);
            }

            bool isMaxLevel = shopData->getLevel("Stat", name) >= shopData->getLevelLimit("Stat", name);
            buyButton->setVisible(!isMaxLevel);
        });

        menuItems.pushBack(statButton);
        index++;
    }

    // Ẩn tất cả label ban đầu
    for (auto* child : panelDescription->getChildren())
        if (dynamic_cast<Label*>(child))
            child->setVisible(false);
}
void ShopScene::updateStatInfo(const std::string& name, Node* panelDescription, bool isMaxLevel)
{
    auto shopData      = ShopSystem::getInstance();
    auto nameLabel     = dynamic_cast<Label*>(panelDescription->getChildByName("nameLabel" + name));
    auto levelLabel    = dynamic_cast<Label*>(panelDescription->getChildByName("levelLabel" + name));
    auto increaseLabel = dynamic_cast<Label*>(panelDescription->getChildByName("increaseLabel" + name));
    auto costLabel     = dynamic_cast<Label*>(panelDescription->getChildByName("costLabel" + name));

    if (!nameLabel || !levelLabel || !increaseLabel || !costLabel)
    {
        AXLOG("Lỗi: Không tìm thấy đầy đủ label cho stat %s", name.c_str());
        return;
    }

    int level             = shopData->getLevel("Stat", name);
    int levelValue        = shopData->getStatLevelValue("Stat", name);
    int cost              = shopData->getCost("Stat", name);
    int levelLimit        = shopData->getLevelLimit("Stat", name);  // Lấy level tối đa
    std::string levelText = isMaxLevel ? "Max" : StringUtils::format("%d/%d", level, levelLimit);

    nameLabel->setString(StringUtils::format("Stat: %s", name.c_str()));
    levelLabel->setString(StringUtils::format("Level: %s", levelText.c_str()));
    increaseLabel->setString(StringUtils::format("Increase: %d", levelValue));
    costLabel->setString(StringUtils::format("Cost: %d", cost));

    // Hiển thị label nếu stat đang được chọn
    if (selectedStatName == name)
    {
        nameLabel->setVisible(true);
        levelLabel->setVisible(true);
        increaseLabel->setVisible(true);
        costLabel->setVisible(true);
    }
}

Label* ShopScene::createStatLabel(const std::string& name,
                                  const std::string& tag,
                                  float baseY,
                                  float xPos,
                                  Node* parent)
{
    auto label = Label::createWithTTF("", "fonts/Pixelpurl-0vBPP.ttf", 20);
    if (!label)
    {
        AXLOG("Lỗi: Không thể tạo label %s", (tag + name).c_str());
        return nullptr;
    }
    label->setAnchorPoint(Vec2(0.0f, 0.5f));
    label->setPosition(xPos, baseY);
    label->setAlignment(TextHAlignment::LEFT);
    label->setVisible(false);
    parent->addChild(label, 4, tag + name);
    return label;
}
