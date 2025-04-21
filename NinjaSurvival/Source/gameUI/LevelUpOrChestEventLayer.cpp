#include "Utils.h"
#include "WeaponUpgradeUtils.h"
#include "LevelUpOrChestEventLayer.h"
#include "systems/LevelSystem.h"
#include "systems/WeaponSystem.h"
#include "systems/SystemManager.h"
#include "scenes/GameScene.h"
#include "AudioManager.h"

using namespace ax;

LevelUpOrChestEventLayer::LevelUpOrChestEventLayer(bool isLevelUp,
                                                   const std::unordered_map<std::string, int>& upgradeList)
    : isLevelUp(isLevelUp), upgradeList(upgradeList), selectedMenuItem(nullptr)
{
    rerollCount = SystemManager::getInstance()->getSystem<LevelSystem>()->getRerollCount();
}

LevelUpOrChestEventLayer* LevelUpOrChestEventLayer::create(bool isLevelUp,
                                                           const std::unordered_map<std::string, int>& upgradeList)
{
    auto layer = new (std::nothrow) LevelUpOrChestEventLayer(isLevelUp, upgradeList);
    if (layer && layer->init())
    {
        layer->autorelease();
        return layer;
    }
    AX_SAFE_DELETE(layer);
    return nullptr;
}

bool LevelUpOrChestEventLayer::init()
{
    if (!Layer::init())
        return false;

    // Tạo nền mờ
    auto background = LayerColor::create(Color4B(0, 0, 0, 128));
    this->addChild(background, 0);

    createUI();

    // Chặn sự kiện chạm phía dưới layer
    auto listener = EventListenerTouchOneByOne::create();
    listener->setSwallowTouches(true);
    listener->onTouchBegan = [](Touch* touch, Event* event) { return true; };
    _eventDispatcher->addEventListenerWithSceneGraphPriority(listener, this);

    return true;
}

void LevelUpOrChestEventLayer::highlightSelectedItem(ax::MenuItemSprite* menuItem)
{
    // Xóa highlight khỏi item trước đó
    if (selectedMenuItem)
    {
        auto prevBorder = selectedMenuItem->getChildByName("selection_border");
        if (prevBorder)
        {
            prevBorder->setVisible(false);
        }
    }

    // Cập nhật item được chọn
    selectedMenuItem = menuItem;

    if (menuItem)
    {
        // Tìm hoặc tạo border
        auto border = menuItem->getChildByName<DrawNode*>("selection_border");
        if (!border)
        {
            border = DrawNode::create();
            border->setName("selection_border");
            auto sprite = menuItem->getNormalImage();
            auto size   = sprite->getContentSize();
            // Vẽ viền màu vàng, độ dày 4 pixel
            border->drawRect(Vec2(0, 0), Vec2(size.width, size.height), Color4F::YELLOW, 4.0f);
            menuItem->addChild(border, 1);
        }
        border->setVisible(true);
    }
}

void LevelUpOrChestEventLayer::createUI()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin      = Director::getInstance()->getVisibleOrigin();

    // Tạo panelLevelUp
    auto panelLevelUp = Sprite::create("UI/panelLevelUp.png");
    panelLevelUp->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    this->addChild(panelLevelUp, 1);
    

    // Tạo vector chứa các menu item
    Vector<MenuItem*> menuItems;

    // Tạo danh sách lựa chọn
    Vector<MenuItem*> selectionItems;
    float itemHeight = 80.0f;
    int maxDisplay   = std::min(static_cast<int>(upgradeList.size()), 3);

    // Kiểm tra nếu upgradeList rỗng
    if (upgradeList.empty())
    {
        AXLOG("'No upgrades available'");
        return;
    }

    int i = 0;
    for (const auto& upgrade : upgradeList)
    {
        if (i >= maxDisplay)
            break;

        // Tạo sprite cho sub-panel
        auto subPanelSprite = Sprite::create("UI/panelChooseUp.png");
        if (!subPanelSprite)
        {
            AXLOG("Error: Failed to load panelChooseUp.png");
            continue;
        }

        // Tạo menu item với sprite sub-panel
        auto menuItem = MenuItemSprite::create(subPanelSprite, subPanelSprite, [=](ax::Object* sender) {
            if (isLevelUp)
            {
                selectedUpgrade = upgrade.first;
                highlightSelectedItem(dynamic_cast<MenuItemSprite*>(sender));  // Sử dụng sender
                if (confirmButton)
                {
                    confirmButton->setVisible(true);
                }
            }
        });
        
        // Tạo văn bản hiển thị
        std::string displayText;
        if (upgrade.second > 0)
        {
            displayText += WeaponUpgradeUtils::getDescription(upgrade.first, upgrade.second);
        }
        

        auto itemLabel = Label::createWithTTF(displayText, "fonts/Pixelpurl-0vBPP.ttf", 20);
        itemLabel->setDimensions(250, 0);  // Hơi nhỏ hơn 300 để có lề
        itemLabel->setAlignment(TextHAlignment::LEFT);
        itemLabel->setAnchorPoint(Vec2(0, 0.5));
        itemLabel->setPosition(Vec2(10, subPanelSprite->getContentSize().height / 2));  // Vị trí với lề trái

        // Thêm nhãn vào menu item
        menuItem->addChild(itemLabel);

        // Định vị menu item
        float startY = (maxDisplay - 1) * itemHeight * 4 / 5;     // Căn giữa danh sách theo chiều dọc
        menuItem->setPosition(Vec2(0, startY - i * itemHeight));  // Căn giữa theo chiều ngang

        if (!isLevelUp)
        {
            // Tự động gán selectedUpgrade cho mục duy nhất
            selectedUpgrade = upgrade.first;
            // Vô hiệu hóa tương tác
            menuItem->setCascadeOpacityEnabled(false);
            menuItem->setCascadeColorEnabled(false);
            menuItem->setEnabled(false);
            subPanelSprite->setColor(Color3B::GRAY);  // Làm xám sub-panel
            highlightSelectedItem(menuItem);          // Highlight mặc định cho mục duy nhất
        }

        selectionItems.pushBack(menuItem);
        i++;
    }

    // Tạo menu cho danh sách lựa chọn
    selectionMenu = Menu::createWithArray(selectionItems);
    selectionMenu->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    this->addChild(selectionMenu, 2);

    // Tạo nút Confirm
    confirmButton = MenuItemImage::create("UI/confirmButton.png", "UI/confirmButton.png",
                                          AX_CALLBACK_1(LevelUpOrChestEventLayer::onConfirm, this));
    confirmButton->setScale(0.7);
    confirmButton->setPosition(
         + panelLevelUp->getContentSize().width * 0.58f,  // Căn giữa ngang
                               panelLevelUp->getContentSize().height * (1.5 / 3));  // 1.5/3 chiều cao

    confirmButton->setVisible(isLevelUp ? false : true);

    // Tạo nút Reroll và label (chỉ khi isLevelUp = true)
    if (isLevelUp)
    {
        rerollButton = MenuItemImage::create("UI/rerollbutton.png", "UI/rerollbutton.png",
                                             AX_CALLBACK_1(LevelUpOrChestEventLayer::onReroll, this));
        rerollButton->setScale(0.7);
        rerollButton->setPosition(panelLevelUp->getContentSize().width * (4.4 / 5),  // 4.4/5 chiều rộng
                                  panelLevelUp->getContentSize().height * (1.73 / 3));

        // Tạo label hiển thị số lần reroll
        rerollCountLabel = Label::createWithTTF(std::to_string(rerollCount), "fonts/Pixelpurl-0vBPP.ttf", 23);
        rerollCountLabel->setPosition(rerollButton->getContentSize().width / 2, rerollButton->getContentSize().height / 2);
        rerollButton->addChild(rerollCountLabel, 20);

        // Nếu rerollCount = 0, làm mờ và vô hiệu hóa nút
        if (rerollCount <= 0)
        {
            rerollButton->setOpacity(128);
            rerollButton->setEnabled(false);
        }
    }

    // Thêm các nút vào menuItems
    menuItems.pushBack(confirmButton);
    if (rerollButton)
    {
        menuItems.pushBack(rerollButton);
    }

    // Tạo menu từ danh sách menu items
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 2);
}

void LevelUpOrChestEventLayer::onReroll(ax::Object* sender)
{
    // Gọi upgradeGenerator để làm mới upgradeList
    upgradeList = SystemManager::getInstance()->getSystem<LevelSystem>()->upgradeGenerator(true);

    // Xóa menu hiện tại
    if (selectionMenu)
    {
        selectionMenu->removeFromParentAndCleanup(true);
        selectionMenu = nullptr;
    }

    // Xóa highlight hiện tại
    selectedMenuItem = nullptr;

    // Tạo lại danh sách mới
    Vector<MenuItem*> newItems;
    float itemHeight = 80.0f;
    int maxDisplay   = std::min(static_cast<int>(upgradeList.size()), 3);

    auto visibleSize = Director::getInstance()->getVisibleSize();

    int i = 0;
    for (const auto& upgrade : upgradeList)
    {
        if (i >= maxDisplay)
            break;

        // Tạo sprite cho sub-panel
        auto subPanelSprite = Sprite::create("UI/panelChooseUp.png");
        if (!subPanelSprite)
        {
            AXLOG("Error: Failed to load panelChooseUp.png");
            continue;
        }

        // Tạo menu item với sprite sub-panel
        auto menuItem = MenuItemSprite::create(subPanelSprite, subPanelSprite, [=](ax::Object* sender) {
            if (isLevelUp)
            {
                selectedUpgrade = upgrade.first;
                highlightSelectedItem(dynamic_cast<MenuItemSprite*>(sender));  // Sử dụng sender
                if (confirmButton)
                {
                    confirmButton->setVisible(true);
                }
            }
        });

        // Tạo văn bản hiển thị
        std::string displayText = upgrade.first;
        if (upgrade.second > 0)
        {
            displayText += " - level " + std::to_string(upgrade.second);
        }
        displayText += "\n" + WeaponUpgradeUtils::getDescription(upgrade.first, upgrade.second);

        auto itemLabel = Label::createWithTTF(displayText, "fonts/Pixelpurl-0vBPP.ttf", 20);
        itemLabel->setDimensions(280, 0);  // Hơi nhỏ hơn 300 để có lề
        itemLabel->setAlignment(TextHAlignment::LEFT);
        itemLabel->setAnchorPoint(Vec2(0, 0.5));
        itemLabel->setPosition(Vec2(10, subPanelSprite->getContentSize().height / 2));  // Vị trí với lề trái

        // Thêm nhãn vào menu item
        menuItem->addChild(itemLabel);

        // Định vị menu item
        float startY = (maxDisplay - 1) * itemHeight * 4 / 5;     // Căn giữa danh sách theo chiều dọc
        menuItem->setPosition(Vec2(0, startY - i * itemHeight));  // Căn giữa theo chiều ngang

        if (!isLevelUp)
        {
            // Tự động gán selectedUpgrade cho mục duy nhất
            selectedUpgrade = upgrade.first;
            menuItem->setCascadeOpacityEnabled(false);
            menuItem->setCascadeColorEnabled(false);
            menuItem->setEnabled(false);
            subPanelSprite->setColor(Color3B::GRAY);  // Làm xám sub-panel
            highlightSelectedItem(menuItem);          // Highlight mặc định cho mục duy nhất
        }

        newItems.pushBack(menuItem);
        i++;
    }

    // Tạo menu mới
    selectionMenu = Menu::createWithArray(newItems);
    selectionMenu->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    this->addChild(selectionMenu, 2);  // Đảm bảo menu ở trên panel

    // Giảm số lần reroll và cập nhật UI
    rerollCount--;
    if (rerollCountLabel)
    {
        rerollCountLabel->setString(std::to_string(rerollCount));
    }
    if (rerollCount <= 0 && rerollButton)
    {
        rerollButton->setOpacity(128);
        rerollButton->setEnabled(false);
    }
}

void LevelUpOrChestEventLayer::onConfirm(ax::Object* sender)
{
    auto gameScene = this->getParent();
    if (gameScene)
    {
        // music click
        AudioManager::getInstance()->playSound("button_click", false, 1.0f, "click");

        if (!selectedUpgrade.empty())
        {
            // Xử lý
            SystemManager::getInstance()->getSystem<WeaponSystem>()->upgradeWeaponAndBuff(selectedUpgrade);
            AXLOG("Upgrade item %s", selectedUpgrade.c_str());
        }
        else
        {
            AXLOG("Khong co upgrade duoc chon");
        }

        //gameScene->scheduleUpdate();  // Tiếp tục update của GameScene
        SystemManager::getInstance()->setUpdateState(true);
    }
    SystemManager::getInstance()->getSystem<LevelSystem>()->setRerollCount(rerollCount);
    this->removeFromParentAndCleanup(true);  // Xóa layer
}
