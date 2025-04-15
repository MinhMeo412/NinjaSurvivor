#include "Utils.h"
#include "WeaponUpgradeUtils.h"
#include "LevelUpOrChestEventLayer.h"
#include "systems/LevelSystem.h"
#include "systems/SystemManager.h"
#include "scenes/GameScene.h"
#include "AudioManager.h"

using namespace ax;

LevelUpOrChestEventLayer::LevelUpOrChestEventLayer(bool isLevelUp,
                                                   const std::vector<std::pair<std::string, int>>& upgradeList)
    : isLevelUp(isLevelUp), upgradeList(upgradeList)
{
    rerollCount = SystemManager::getInstance()->getSystem<LevelSystem>()->getRerollCount();
}

LevelUpOrChestEventLayer* LevelUpOrChestEventLayer::create(bool isLevelUp,
                                                           const std::vector<std::pair<std::string, int>>& upgradeList)
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

void LevelUpOrChestEventLayer::createUI()
{
    auto visibleSize = Director::getInstance()->getVisibleSize();
    auto origin      = Director::getInstance()->getVisibleOrigin();

    // Tạo panel

    // Tạo vector chứa các menu item
    Vector<MenuItem*> menuItems;

    // Tạo danh sách lựa chọn
    Vector<MenuItem*> selectionItems;
    float itemHeight = 80.0f;
    int maxDisplay   = std::min(static_cast<int>(upgradeList.size()), 3);

    // Kiểm tra nếu upgradeList rỗng, thêm "No upgrades available" vào danh sách
    if (upgradeList.empty())
    {
        AXLOG("createUI - upgradeList empty, adding 'No upgrades available'");
        upgradeList.push_back({"No upgrades available", 0});
    }

    for (int i = 0; i < maxDisplay; i++)
    {
        // Tạo văn bản hiển thị với định dạng yêu cầu
        std::string displayText = upgradeList[i].first;
        if (upgradeList[i].second > 0)
        {
            displayText += " - level " + std::to_string(upgradeList[i].second);
        }
        displayText += "\n" + WeaponUpgradeUtils::getDescription(upgradeList[i].first, upgradeList[i].second);

        auto itemLabel = Label::createWithTTF(displayText, "fonts/Marker Felt.ttf", 24);
        itemLabel->setDimensions(300, 0);               // Giới hạn chiều rộng, cho phép xuống dòng
        itemLabel->setAlignment(TextHAlignment::LEFT);  // Căn trái văn bản

        auto menuItem = MenuItemLabel::create(itemLabel, [=](Object* sender) {
            if (isLevelUp)
            {
                selectedUpgrade = upgradeList[i].first;
                if (confirmButton)
                {
                    confirmButton->setVisible(true);
                }
            }
        });
        menuItem->setPosition(Vec2(0, 50 - i * itemHeight));
        if (!isLevelUp)
        {
            // Vô hiệu hóa tương tác nhưng giữ màu bình thường
            menuItem->setCascadeOpacityEnabled(false);
            menuItem->setCascadeColorEnabled(false);
            menuItem->setEnabled(false);
            if (itemLabel)
            {
                itemLabel->setTextColor(Color4B::WHITE);  // Đảm bảo màu chữ không đổi
            }
        }
        selectionItems.pushBack(menuItem);
    }

    // Tạo menu cho danh sách lựa chọn
    selectionMenu = Menu::createWithArray(selectionItems);
    selectionMenu->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    this->addChild(selectionMenu, 2);

    // Tạo nút Confirm
    confirmButton = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",
                                          AX_CALLBACK_1(LevelUpOrChestEventLayer::onConfirm, this));
    confirmButton->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 150));
    confirmButton->setVisible(isLevelUp ? false : true);

    // Tạo nút Reroll và label (chỉ khi isLevelUp = true)
    if (isLevelUp)
    {
        rerollButton = MenuItemImage::create("CloseNormal.png", "CloseSelected.png",
                                             AX_CALLBACK_1(LevelUpOrChestEventLayer::onReroll, this));
        rerollButton->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 100));

        // Tạo label hiển thị số lần reroll
        rerollCountLabel = Label::createWithTTF("Reroll: " + std::to_string(rerollCount), "fonts/Marker Felt.ttf", 20);
        rerollCountLabel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 - 130));
        this->addChild(rerollCountLabel, 2);

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
    //upgradeList = SystemManager::getInstance()->getSystem<LevelSystem>()->upgradeGenerator();

    // Xóa menu hiện tại
    if (selectionMenu)
    {
        selectionMenu->removeFromParentAndCleanup(true);
        selectionMenu = nullptr;
    }

    // Tạo lại danh sách mới
    Vector<MenuItem*> newItems;
    float itemHeight  = 80.0f;
    int newMaxDisplay = std::min(static_cast<int>(upgradeList.size()), 3);

    for (int i = 0; i < newMaxDisplay; i++)
    {
        // Tạo văn bản hiển thị với định dạng yêu cầu
        std::string displayText = upgradeList[i].first;
        if (upgradeList[i].second > 0)
        {
            displayText += " - level " + std::to_string(upgradeList[i].second);
        }
        displayText += "\n" + WeaponUpgradeUtils::getDescription(upgradeList[i].first, upgradeList[i].second);

        auto itemLabel = Label::createWithTTF(displayText, "fonts/Marker Felt.ttf", 24);
        itemLabel->setDimensions(300, 0);               // Giới hạn chiều rộng, cho phép xuống dòng
        itemLabel->setAlignment(TextHAlignment::LEFT);  // Căn trái văn bản

        auto menuItem = MenuItemLabel::create(itemLabel, [=](Object* sender) {
            if (isLevelUp)
            {
                selectedUpgrade = upgradeList[i].first;
                if (confirmButton)
                {
                    confirmButton->setVisible(true);
                }
            }
        });
        menuItem->setPosition(Vec2(0, 50 - i * itemHeight));
        if (!isLevelUp)
        {
            // Vô hiệu hóa tương tác nhưng giữ màu bình thường
            menuItem->setCascadeOpacityEnabled(false);
            menuItem->setCascadeColorEnabled(false);
            menuItem->setEnabled(false);
            if (itemLabel)
            {
                itemLabel->setTextColor(Color4B::WHITE);  // Đảm bảo màu chữ không đổi
            }
        }
        newItems.pushBack(menuItem);
    }

    // Tạo menu mới
    auto visibleSize = Director::getInstance()->getVisibleSize();
    selectionMenu    = Menu::createWithArray(newItems);
    selectionMenu->setPosition(visibleSize.width / 2, visibleSize.height / 2);
    this->addChild(selectionMenu, 2);  // Đảm bảo menu ở trên panel

    // Giảm số lần reroll và cập nhật UI
    rerollCount--;
    if (rerollCountLabel)
    {
        rerollCountLabel->setString("Reroll: " + std::to_string(rerollCount));
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

        if (isLevelUp && !selectedUpgrade.empty())
        {
            // Xử lý
            // SystemManager::getInstance()->getSystem<LevelSystem>()
        }

        gameScene->scheduleUpdate();  // Tiếp tục update của GameScene
    }
    SystemManager::getInstance()->getSystem<LevelSystem>()->setRerollCount(rerollCount);
    this->removeFromParentAndCleanup(true);  // Xóa layer
}
