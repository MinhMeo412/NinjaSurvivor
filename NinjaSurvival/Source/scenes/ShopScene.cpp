#include "ShopScene.h"
#include "Utils.h"
#include "MainScene.h"
#include "systems/ShopSystem.h"

using namespace ax;

ShopScene::ShopScene() : SceneBase("ShopScene"), selectedStatItem(nullptr), selectedStatName(""), buyButton(nullptr) {}

bool ShopScene::init()
{
    if (!SceneBase::init())
    {
        return false;
    }

    // Tải dữ liệu từ savegame.json
    auto shopData = ShopSystem::getInstance();
    if (!shopData->loadSaveGame())
    {
        AXLOG("Failed to load savegame.json in ShopScene::init()");
    }
    else
    {
        AXLOG("Successfully loaded savegame.json in ShopScene::init()");
    }

    menuUISetup();

    return true;
}

void ShopScene::update(float dt) {}

void ShopScene::menuUISetup()
{
    float marginX = visibleSize.width * 0.05f;    // 5% margin left
    float marginY = visibleSize.height * 0.025f;  // 2.5% margin top


    // Vẽ viền vùng an toàn
    auto drawNode = DrawNode::create();
    drawNode->setPosition(Vec2::ZERO);
    addChild(drawNode);
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4F::BLUE);

    // Background
    auto background = Sprite::create("UI/background3.png");
    if (background)
    {
        background->setPosition(Vec2(origin.x + visibleSize.width * 0.5f, origin.y + visibleSize.height * 0.5f));
        this->addChild(background, 0);
    }

    // Panel chính
    auto panel = Sprite::create("UI/panel4.png");
    if (!panel)
    {
        AXLOG("Failed to create panel4.png");
        return;
    }
    panel->setPosition(Vec2(origin.x + visibleSize.width * 0.5f, origin.y + visibleSize.height * 0.5f));
    this->addChild(panel, 2);

    // Panel mô tả
    auto panelDescription = Sprite::create("UI/NeoPanelDecripsionInSkillUp2.png");
    if (!panelDescription)
    {
        AXLOG("Failed to create NeoPanelDecripsionInSkillUp2.png");
        return;
    }
    float descY = panel->getPositionY() - panel->getContentSize().height * 0.45f +
                  panel->getContentSize().height * 0.2f - panelDescription->getContentSize().height * 0.5f;
    panelDescription->setPosition(Vec2(origin.x + visibleSize.width * 0.5f, descY));
    this->addChild(panelDescription, 3);

    // Hiển thị số coin và UI coin ở phía trên bên phải
    auto shopData = ShopSystem::getInstance();
    // Tạo nhãn hiển thị số coin
    auto coinLabel = Label::createWithTTF(StringUtils::format("%d", shopData->getCoins()), "fonts/Marker Felt.ttf", 24);
    if (!coinLabel)
    {
        AXLOG("Failed to create coinLabel");
        return;
    }

    // Tạo UI coin từ asset
    auto coinSprite = Sprite::create("coin.png");  // Giả sử asset coin có tên "coin.png"
    if (!coinSprite)
    {
        AXLOG("Failed to create coinSprite");
        return;
    }

    // Đặt vị trí cho coinLabel và coinSprite
    float coinLabelWidth  = coinLabel->getContentSize().width;
    float coinSpriteWidth = coinSprite->getContentSize().width;
    float totalWidth      = coinLabelWidth + coinSpriteWidth + 10.0f;  // Khoảng cách 10px giữa nhãn và sprite
    float startPosX       = origin.x + visibleSize.width - marginX - totalWidth;  // Bắt đầu từ bên phải, trừ margin
    float posY            = origin.y + visibleSize.height - marginY;              // Vị trí Y giữ nguyên (phía trên)

    // Đặt vị trí cho coinLabel (bên trái)
    coinLabel->setPosition(Vec2(startPosX + coinLabelWidth / 2, posY));
    coinLabel->setAlignment(TextHAlignment::LEFT);  // Căn trái để số coin hiển thị tự nhiên
    this->addChild(coinLabel, 5, "coinLabel");

    // Đặt vị trí cho coinSprite (bên phải của coinLabel)
    coinSprite->setPosition(Vec2(startPosX + coinLabelWidth + 10.0f + coinSpriteWidth / 2, posY));
    coinSprite->setScale(3);
    this->addChild(coinSprite, 5, "coinSprite");

    // Menu items
    Vector<MenuItem*> menuItems;

    // Nút Back
    closeItem = Utils::createMenuItem("UI/buttonback.png", "UI/buttonback.png",
                                      AX_CALLBACK_1(ShopScene::menuReturnCallback, this),
                                      Vec2(origin.x + marginX, origin.y + visibleSize.height - marginY));
    if (closeItem)
        menuItems.pushBack(closeItem);

    // Load dữ liệu từ ShopSystem
    const auto& shopItems = shopData->getShopData();

    // Tạo nút stat trong panel chính
    int index = 0;
    for (const auto& item : shopItems)
    {
        if (item.type != "Stat")
            continue;

        // Tạo nhãn thông tin stat
        auto statInfoLabel = Label::createWithTTF("", "fonts/Marker Felt.ttf", 20);
        if (!statInfoLabel)
        {
            AXLOG("Failed to create statInfoLabel for %s", item.name.c_str());
            continue;
        }
        statInfoLabel->setPosition(
            Vec2(panelDescription->getContentSize().width * 0.22f, panelDescription->getContentSize().height * 0.7f));
        statInfoLabel->setAlignment(TextHAlignment::LEFT);
        statInfoLabel->setVisible(false);
        panelDescription->addChild(statInfoLabel, 4, "label" + item.name);

        // Kiểm tra nếu đạt level tối đa
        bool isMaxLevel =
            item.level.has_value() && item.levelLimit.has_value() && item.level.value() >= item.levelLimit.value();
        std::string levelText = isMaxLevel ? "Max" : StringUtils::format("%d", item.level.value_or(0));
        statInfoLabel->setString(StringUtils::format(
            "Stat: %s\nLevel: %s\nIncrease: %d\nCost: %d", item.name.empty() ? "Unknown" : item.name.c_str(),
            levelText.c_str(), item.levelValue.value_or(0), item.cost.value_or(50)));

        std::string spritePath = item.name + ".png";
        std::string statValue  = StringUtils::format(
            "Lv.%s", isMaxLevel ? "Max" : StringUtils::format("%d", item.level.value_or(0)).c_str());
        auto statButton = Utils::createStatButton(spritePath, statValue);
        if (!statButton)
        {
            AXLOG("Failed to create statButton for %s", item.name.c_str());
            continue;
        }

        const int cols     = 4;
        float iconSpacingX = panel->getContentSize().width * 0.08f;
        float startX =
            panel->getPositionX() - panel->getContentSize().width * 0.4f + statButton->getContentSize().width * 0.5f;
        float buttonX = startX + index * (statButton->getContentSize().width + iconSpacingX);
        float buttonY = panel->getPositionY() + panel->getContentSize().height * 0.43f;
        statButton->setScale(1.5);
        statButton->setPosition(Vec2(buttonX, buttonY));

        // Callback khi chọn stat
        statButton->setCallback([this, itemName = item.name, itemLevel = item.level, itemValue = item.levelValue,
                                 itemCost = item.cost, itemLevelLimit = item.levelLimit,
                                 panelDescription](ax::Object* sender) {
            AXLOG("Stat clicked: %s, selectedStatItem: %p", itemName.c_str(), selectedStatItem);

            if (selectedStatItem != nullptr && selectedStatItem != sender && selectedStatItem->getParent() != nullptr)
            {
                if (auto* oldLabel = dynamic_cast<Label*>(selectedStatItem->getChildByName("label")))
                    oldLabel->setVisible(false);
            }

            selectedStatName = itemName;
            selectedStatItem = dynamic_cast<MenuItemSprite*>(sender);

            for (auto* child : panelDescription->getChildren())
                if (dynamic_cast<Label*>(child))
                    child->setVisible(false);

            if (auto* label = dynamic_cast<Label*>(panelDescription->getChildByName("label" + itemName)))
                label->setVisible(true);

            if (selectedStatItem != nullptr && selectedStatItem->getParent() != nullptr)
            {
                if (auto* buttonLabel = dynamic_cast<Label*>(selectedStatItem->getChildByName("label")))
                    buttonLabel->setVisible(true);
            }

            // Kiểm tra nếu đạt level tối đa thì ẩn nút Buy
            bool isMaxLevel =
                itemLevel.has_value() && itemLevelLimit.has_value() && itemLevel.value() >= itemLevelLimit.value();
            if (buyButton)
                buyButton->setVisible(!isMaxLevel);
        });

        menuItems.pushBack(statButton);
        index++;
    }

    // Đảm bảo tất cả nhãn trong panelDescription đều ẩn khi khởi tạo
    for (auto* child : panelDescription->getChildren())
        if (dynamic_cast<Label*>(child))
            child->setVisible(false);

    // Nút Buy
    float panelDescRight  = panelDescription->getPositionX() + panelDescription->getContentSize().width * 0.5f;
    float panelDescBottom = panelDescription->getPositionY() - panelDescription->getContentSize().height * 0.5f;
    auto buySprite        = ax::Sprite::create("UI/buttonBuy.png");
    if (!buySprite)
    {
        AXLOG("Failed to create buySprite");
        return;
    }
    float buyX = panelDescRight - (buySprite->getContentSize().width * 0.5f * 0.8f) -
                 panelDescription->getContentSize().width * 0.05f;
    float buyY = panelDescription->getPositionY() - panelDescription->getContentSize().height * 0.25f +
                 buySprite->getContentSize().height * 0.3f;

    AXLOG("Buy button position: x=%f, y=%f", buyX, buyY);
    buyButton = Utils::createButton("UI/buttonBuy.png", [this, shopData, panelDescription](ax::Object* sender) {
        int currentCoins = shopData->getCoins();
        int cost         = shopData->getCost("Stat", this->selectedStatName);  // Lấy cost từ ShopData
        int increase =
            shopData->getValueIncrease("Stat", this->selectedStatName) * 100;  // Giả sử valueIncrease là phần trăm

        if (currentCoins >= cost && !this->selectedStatName.empty())
        {
            shopData->setCoins(currentCoins - cost);
            const auto& shopItems = shopData->getShopData();

            for (const auto& item : shopItems)
            {
                if (item.type == "Stat" && item.name == this->selectedStatName)
                {
                    int level           = item.level.value_or(0);
                    int value           = item.levelValue.value_or(0);
                    float valueIncrease = item.valueIncrease.value_or(0.1f);
                    bool isMaxLevel     = item.levelLimit.has_value() && (level + 1) >= item.levelLimit.value();

                    // Cập nhật stat
                    shopData->setStatLevel(item.name, level + 1, value + increase,
                                           valueIncrease + 0.01f);  // Tăng valueIncrease

                    // Cập nhật nhãn trên nút stat
                    if (this->selectedStatItem != nullptr && this->selectedStatItem->getParent() != nullptr)
                    {
                        if (auto* label = dynamic_cast<Label*>(this->selectedStatItem->getChildByName("label")))
                            label->setString(StringUtils::format(
                                "Lv.%s", isMaxLevel ? "Max" : StringUtils::format("%d", level + 1).c_str()));
                    }

                    // Cập nhật nhãn thông tin stat
                    if (auto* statLabel = dynamic_cast<Label*>(panelDescription->getChildByName("label" + item.name)))
                        statLabel->setString(
                            StringUtils::format("Stat: %s\nLevel: %s\nIncrease: %d\nCost: %d",
                                                item.name.empty() ? "Unknown" : item.name.c_str(),
                                                isMaxLevel ? "Max" : StringUtils::format("%d", level + 1).c_str(),
                                                value + increase, item.cost.value_or(50)));

                    // Ẩn nút Buy nếu đạt level tối đa
                    if (isMaxLevel && buyButton)
                        buyButton->setVisible(false);

                    break;
                }
            }

            // Cập nhật số coin hiển thị
            if (auto* coinLabel = dynamic_cast<Label*>(this->getChildByName("coinLabel")))
                coinLabel->setString(StringUtils::format("Coins: %d", shopData->getCoins()));

            shopData->saveToFile(ax::FileUtils::getInstance()->getWritablePath() + "savegame.json");
            AXLOG("Upgraded stat: %s", this->selectedStatName.c_str());
        }
        else
        {
            AXLOG("Not enough coins or no stat selected!");
        }
    }, Vec2(buyX, buyY), 0.8f, false);  // Ẩn ban đầu
    if (buyButton)
    {
        menuItems.pushBack(buyButton);
    }
    else
    {
        AXLOG("Failed to create buyButton");
    }

    // Tạo menu chứa các nút
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 10);
}

void ShopScene::menuReturnCallback(ax::Object* sender)
{
    auto scene = utils::createInstance<MainScene>();
    if (scene)
        _director->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}
