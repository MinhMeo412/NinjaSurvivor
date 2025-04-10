#include "Utils.h"
#include "CharacterChooseScene.h"
#include "MapChooseScene.h"
#include "MainScene.h"
#include "systems/GameData.h"
#include "systems/ShopSystem.h"
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
    //systemManager->resetSystems();


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

    // Tắt input
    _director->getEventDispatcher()->setEnabled(false);

    //Truyền tên nhân vật được chọn cho GameData lưu trữ
    GameData::getInstance()->setSelectedCharacter(selectedCharacterName);

    //Đổi scene
    auto scene = utils::createInstance<MapChooseScene>();
    _director->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));

    // Bật lại input sau khi chuyển xong
    this->scheduleOnce([this](float dt) { _director->getEventDispatcher()->setEnabled(true); }, TRANSITION_TIME,
                       "enableInput");
}


void CharacterChooseScene::menuUISetup()
{
    float marginX = visibleSize.width * 0.05f;    // Lề trái 5% chiều rộng màn hình
    float marginY = visibleSize.height * 0.025f;  // Lề trên 2.5% chiều cao màn hình

    // Tạo và đặt background ở giữa màn hình
    auto background = Sprite::create("UI/background3.png");
    background->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
    this->addChild(background, 0);  // Thêm vào scene với z-order 0 (nền)

    // Tạo panel chọn nhân vật, đặt gần đỉnh màn hình (cách 5% từ trên)
    auto panelChooseCharacter = Sprite::create("UI/NeopanelChoseCharacter2.png");
    float panelY = origin.y + visibleSize.height * 0.95f - panelChooseCharacter->getContentSize().height / 2;
    panelChooseCharacter->setPosition(Vec2(origin.x + visibleSize.width / 2, panelY));
    this->addChild(panelChooseCharacter, 2);  // Thêm với z-order 2 (trên background)

    // Vẽ viền vùng an toàn bằng DrawNode
    auto drawNode = DrawNode::create();
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4F::BLUE);
    this->addChild(drawNode);  // Thêm vào scene (z-order mặc định 0)

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
    float startPosX          = origin.x + visibleSize.width - marginX - totalWidth;  // Bắt đầu từ bên phải, trừ margin
    float posY            = origin.y + visibleSize.height - marginY;              // Vị trí Y giữ nguyên (phía trên)

    // Đặt vị trí cho coinLabel (bên trái)
    coinLabel->setPosition(Vec2(startPosX + coinLabelWidth / 2, posY));
    coinLabel->setAlignment(TextHAlignment::LEFT);  // Căn trái để số coin hiển thị tự nhiên
    this->addChild(coinLabel, 5, "coinLabel");

    // Đặt vị trí cho coinSprite (bên phải của coinLabel)
    coinSprite->setPosition(Vec2(startPosX + coinLabelWidth + 10.0f + coinSpriteWidth / 2, posY));
    coinSprite->setScale(3);
    this->addChild(coinSprite, 5, "coinSprite");

    // Khởi tạo vector chứa các menu item
    Vector<MenuItem*> menuItems;

    // Tạo nút Return ở góc trên trái
    returnButton = Utils::createMenuItem("UI/buttonback.png", "UI/buttonback.png",
                                         AX_CALLBACK_1(CharacterChooseScene::menuReturnCallback, this),
                                         Vec2(origin.x + marginX, origin.y + visibleSize.height - marginY));
    menuItems.pushBack(returnButton);  // Thêm nút Return vào danh sách menu items

    // Tạo panel mô tả, đặt bên dưới panelChooseCharacter (cách 1% chiều cao màn hình)
    auto panelDescription = Sprite::create("UI/NeoDescriptionCharacter.png");
    float descY           = panelChooseCharacter->getPositionY() - panelChooseCharacter->getContentSize().height / 2 -
                  visibleSize.height * 0.01f - panelDescription->getContentSize().height / 2;
    panelDescription->setPosition(Vec2(origin.x + visibleSize.width / 2, descY));
    this->addChild(panelDescription, 2);  // Thêm với z-order 2

    // Tính toán vị trí cho nút Next/Buy
    float panelDescRight  = panelDescription->getPositionX() + panelDescription->getContentSize().width / 2;
    float panelDescBottom = panelDescription->getPositionY() - panelDescription->getContentSize().height / 2;
    auto nextSprite       = Sprite::create("UI/buttonNext.png");  // Tạo sprite mẫu để lấy kích thước
    float playX           = panelDescRight - (nextSprite->getContentSize().width / 2 * 0.8f) -
                  panelDescription->getContentSize().width * 0.05f;
    float playY = panelDescBottom + (nextSprite->getContentSize().height / 2 * 0.8f) +
                  panelDescription->getContentSize().height * 0.05f;

    // Tạo nút Next, ban đầu ẩn
    auto nextButton =
        Utils::createButton("UI/buttonNext.png", AX_CALLBACK_1(CharacterChooseScene::menuNextCallback, this),
                            Vec2(playX, playY), 0.8f, false);
    menuItems.pushBack(nextButton);

    // Tạo nút Buy với logic mua nhân vật, ban đầu ẩn
    auto buyButton = Utils::createButton("UI/buttonBuy.png", [this, nextButton, panelDescription](ax::Object* sender) {
        auto shopData    = ShopSystem::getInstance();
        auto gameData    = GameData::getInstance();
        int currentCoins = shopData->getCoins();
        int cost         = shopData->getCost("entities", selectedCharacterName);  // Lấy cost từ ShopSystem

        // Kiểm tra đủ điều kiện mua
        if (!selectedCharacterName.empty() && currentCoins >= cost)
        {
            shopData->setCoins(currentCoins - cost);                          // Trừ coin
            shopData->setAvailable("entities", selectedCharacterName, true);  // Mở khóa trong shop
            gameData->setCharacterAvailable(selectedCharacterName, true);     // Mở khóa trong game
            shopData->syncCharactersWithGameData();                           // Đồng bộ dữ liệu

            if (selectedCharacterItem)  // Nếu có nhân vật được chọn
            {
                // Cập nhật trạng thái available trong GameData
                auto& entities =
                    const_cast<std::unordered_map<std::string, std::unordered_map<std::string, EntityTemplate>>&>(
                        gameData->getEntityTemplates());
                if (entities["player"].find(selectedCharacterName) != entities["player"].end())
                {
                    entities["player"][selectedCharacterName].available = true;
                }

                // Cập nhật giao diện ngay lập tức
                Utils::updateItemUI(selectedCharacterItem, panelDescription, true);  // Bỏ mờ nhân vật
                dynamic_cast<Node*>(sender)->setVisible(false);                      // Ẩn nút Buy
                nextButton->setVisible(true);                                        // Hiện nút Next

                // Cập nhật statsLabel để hiển thị chỉ số sau khi unlock
                auto statsLabel =
                    dynamic_cast<Label*>(panelDescription->getChildByName("statsLabel" + selectedCharacterName));
                if (statsLabel)
                {
                    auto entities         = gameData->getEntityTemplates();
                    auto templ            = entities.at("player").at(selectedCharacterName);
                    std::string statsText = "";
                    int healthIncrease =
                        shopData->getValueIncrease("Stat", "Health") * 100;  // Giả sử valueIncrease là phần trăm
                    int attackIncrease = shopData->getValueIncrease("Stat", "Attack") * 100;
                    if (templ.health.has_value())
                        statsText += StringUtils::format("Health: %.0f\n", templ.health->maxHealth + healthIncrease);
                    if (templ.attack.has_value())
                        statsText += StringUtils::format("Attack: %.0f\n", templ.attack->baseDamage + attackIncrease);
                    if (templ.speed.has_value())
                        statsText += StringUtils::format("Speed: %.0f", templ.speed->speed);
                    statsLabel->setString(statsText);
                    statsLabel->setVisible(true);  // Đảm bảo statsLabel hiển thị
                }
            }

            // Cập nhật số coin hiển thị
            if (auto* coinLabel = dynamic_cast<Label*>(this->getChildByName("coinLabel")))
                coinLabel->setString(StringUtils::format("Coins: %d", shopData->getCoins()));

            // Lưu vào file save
            shopData->saveToFile(ax::FileUtils::getInstance()->getWritablePath() + "savegame.json");

            AXLOG("Unlocked character: %s", selectedCharacterName.c_str());
        }
        else
        {
            AXLOG("Not enough coins or no character selected!");
            dynamic_cast<Node*>(sender)->setVisible(false);  // Ẩn nút Buy nếu thất bại
        }
    }, Vec2(playX, playY), 0.8f, false);
    menuItems.pushBack(buyButton);  // Thêm nút Buy vào danh sách menu items

    // Lấy dữ liệu nhân vật từ GameData
    auto gameData        = GameData::getInstance();
    const auto& entities = gameData->getEntityTemplates();
    int numCharacters    = entities.count("player") ? entities.at("player").size() : 0;  // Đếm số nhân vật
    const int cols       = 2;                                                            // Số cột hiển thị nhân vật
    const int rows       = (numCharacters + cols - 1) / cols;                            // Số hàng, làm tròn lên
    float iconSpacingX   = panelChooseCharacter->getContentSize().width * 0.06f;   // Khoảng cách ngang giữa các button
    float iconSpacingY   = panelChooseCharacter->getContentSize().height * 0.03f;  // Khoảng cách dọc

    // Tính vị trí ban đầu cho các button nhân vật
    float panelLeft = panelChooseCharacter->getPositionX() - panelChooseCharacter->getContentSize().width / 2.4f;
    float startY =
        panelChooseCharacter->getPositionY() + panelChooseCharacter->getContentSize().height / 2.5f - iconSpacingY;
    float startX = panelLeft + iconSpacingX;

    int index = 0;
    if (entities.count("player"))
    {
        for (const auto& [name, templ] : entities.at("player"))
        {
            // Tạo nhãn tên nhân vật trong panelDescription
            auto characterLabel = Label::createWithTTF(name, "fonts/Marker Felt.ttf", 24);
            characterLabel->setPosition(Vec2(panelDescription->getContentSize().width * 0.15f,
                                             panelDescription->getContentSize().height * 0.85f));
            characterLabel->setVisible(false);
            panelDescription->addChild(characterLabel, 6, "label" + name);

            // Tạo nhãn chỉ số nhân vật (statsLabel) hoặc nhãn cost (costLabel) bên dưới characterLabel
            auto statsLabel = Label::createWithTTF("", "fonts/Marker Felt.ttf", 20);
            float characterLabelBottomY =
                panelDescription->getContentSize().height * 0.85f - characterLabel->getContentSize().height * 0.5f;
            float spacing = 20.0f;  // Khoảng cách giữa characterLabel và statsLabel/costLabel
            float statsLabelY =
                characterLabelBottomY - spacing;  // Đặt cạnh trên của statsLabel ngay dưới characterLabel
            statsLabel->setPosition(Vec2(panelDescription->getContentSize().width * 0.18f,
                                         statsLabelY - statsLabel->getContentSize().height * 0.8f));
            statsLabel->setAlignment(TextHAlignment::LEFT);
            statsLabel->setVisible(false);
            panelDescription->addChild(statsLabel, 6, "statsLabel" + name);

            // Nếu nhân vật chưa unlock, hiển thị cost thay vì stats
            if (!templ.available)
            {
                int cost = shopData->getCost("entities", name);
                statsLabel->setString(StringUtils::format("Cost to unlock: %d", cost));
            }
            else
            {
                // Lấy chỉ số cơ bản và cộng thêm từ ShopSystem
                int healthIncrease =
                    shopData->getValueIncrease("Stat", "Health") * 100;  // Giả sử valueIncrease là phần trăm
                int attackIncrease    = shopData->getValueIncrease("Stat", "Attack") * 100;
                std::string statsText = "";
                if (templ.health.has_value())
                    statsText += StringUtils::format("Health: %.0f\n", templ.health->maxHealth + healthIncrease);
                if (templ.attack.has_value())
                    statsText += StringUtils::format("Attack: %.0f\n", templ.attack->baseDamage + attackIncrease);
                if (templ.speed.has_value())
                    statsText += StringUtils::format("Speed: %.0f", templ.speed->speed);
                statsLabel->setString(statsText);
            }

            // Tạo button nhân vật
            auto button =
                Utils::createCharacterButton(templ.sprite ? templ.profilePhoto.value() : "CloseNormal.png", name,
                                             templ.available, selectedCharacterName, selectedCharacterItem, nextButton);

            // Tạo bản sao của name để capture vào lambda
            std::string characterName = name;
            button->setCallback([this, characterName, buyButton, nextButton, characterLabel, statsLabel,
                                 panelDescription](ax::Object* sender) {
                selectedCharacterName = characterName;
                if (selectedCharacterItem)
                    selectedCharacterItem->setEnabled(true);
                selectedCharacterItem = dynamic_cast<MenuItemSprite*>(sender);
                selectedCharacterItem->setEnabled(false);

                // Ẩn tất cả nhãn trong panelDescription trước khi hiển thị nhãn mới
                for (auto* child : panelDescription->getChildren())
                    if (dynamic_cast<Label*>(child))
                        child->setVisible(false);

                // Hiển thị cả characterLabel và statsLabel của nhân vật được chọn
                characterLabel->setVisible(true);
                statsLabel->setVisible(true);

                // Cập nhật giao diện dựa trên trạng thái available từ GameData
                auto gameData    = GameData::getInstance();
                auto shopData    = ShopSystem::getInstance();
                bool isAvailable = gameData->getEntityTemplates().at("player").at(characterName).available;

                // Cập nhật statsLabel dựa trên trạng thái available
                if (!isAvailable)
                {
                    int cost = shopData->getCost("entities", characterName);
                    statsLabel->setString(StringUtils::format("Cost to unlock: %d", cost));
                }
                else
                {
                    auto entities         = gameData->getEntityTemplates();
                    auto templ            = entities.at("player").at(characterName);
                    int healthIncrease    = shopData->getValueIncrease("Stat", "Health") * 100;
                    int attackIncrease    = shopData->getValueIncrease("Stat", "Attack") * 100;
                    std::string statsText = "";
                    if (templ.health.has_value())
                        statsText += StringUtils::format("Health: %.0f\n", templ.health->maxHealth + healthIncrease);
                    if (templ.attack.has_value())
                        statsText += StringUtils::format("Attack: %.0f\n", templ.attack->baseDamage + attackIncrease);
                    if (templ.speed.has_value())
                        statsText += StringUtils::format("Speed: %.0f", templ.speed->speed);
                    statsLabel->setString(statsText);
                }

                // Cập nhật giao diện button
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

    // Tạo menu từ danh sách menu items và thêm vào scene
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);  // Đặt menu ở gốc tọa độ
    this->addChild(menu, 6);        // Thêm với z-order 6 (trên các panel)
}
