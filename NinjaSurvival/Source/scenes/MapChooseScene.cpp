#include "Utils.h"
#include "MapChooseScene.h"
#include "MainScene.h"
#include "GameScene.h"
#include "GameLoadingScene.h"
#include "systems/GameData.h"
#include "systems/ShopSystem.h"


using namespace ax;

MapChooseScene::MapChooseScene() : SceneBase("MapChooseScene") {}

bool MapChooseScene::init()
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

void MapChooseScene::update(float dt) {}

void MapChooseScene::menuCloseCallback(ax::Object* sender)
{
    auto scene = utils::createInstance<MainScene>();
    _director->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}

void MapChooseScene::menuPlayCallback(ax::Object* sender)
{
    if (selectedMapName.empty())
    {
        AXLOG("Error: No map selected");
        return;
    }

    // Tắt input
    _director->getEventDispatcher()->setEnabled(false);

    //Truyền dữ liệu tên map được chọn cho GameData lưu
    GameData::getInstance()->setSelectedMap(selectedMapName);

    // GameScene
    auto gameScene = utils::createInstance<GameScene>();

    // Tạo GameLoadingScene
    auto loadingScene = GameLoadingScene::create();

    // Gán GameScene làm nextScene
    loadingScene->setNextScene(gameScene);

    _director->replaceScene(TransitionFade::create(TRANSITION_TIME, loadingScene));

    // Bật lại input sau khi chuyển xong
    this->scheduleOnce([this](float dt) { _director->getEventDispatcher()->setEnabled(true); }, TRANSITION_TIME,
                       "enableInput");
}

void MapChooseScene::menuUISetup()
{
    float marginX = visibleSize.width * 0.05f;    // Lề trái 5% chiều rộng màn hình
    float marginY = visibleSize.height * 0.025f;  // Lề trên 2.5% chiều cao màn hình

    // Vẽ viền vùng an toàn bằng DrawNode
    auto drawNode = DrawNode::create();
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4F::BLUE);
    this->addChild(drawNode);  // Thêm vào scene (z-order mặc định 0)

    // Tạo và đặt background ở giữa màn hình
    auto background = Sprite::create("UI/background3.png");
    background->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
    this->addChild(background, 0);  // Thêm với z-order 0 (nền)

    // Tạo panel map, đặt ở giữa màn hình
    auto panel = Sprite::create("UI/panelMap.png");
    panel->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
    this->addChild(panel, 2);  // Thêm với z-order 2 (trên background)

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

    // Khởi tạo vector chứa các menu item và định nghĩa lề
    Vector<MenuItem*> menuItems;

    // Tạo nút Close ở góc trên trái
    closeItem = Utils::createMenuItem("UI/buttonback.png", "UI/buttonback.png",
                                      AX_CALLBACK_1(MapChooseScene::menuCloseCallback, this),
                                      Vec2(origin.x + marginX, origin.y + visibleSize.height - marginY));
    menuItems.pushBack(closeItem);  // Thêm nút Close vào danh sách menu items

    // Tính toán vị trí nút Play/Buy ở góc dưới phải của panel
    float panelRight  = panel->getPositionX() + panel->getContentSize().width / 2;
    float panelBottom = panel->getPositionY() - panel->getContentSize().height / 2;
    auto playSprite   = Sprite::create("UI/buttonPlayGame.png");  // Tạo sprite mẫu để lấy kích thước
    float playX = panelRight - (playSprite->getContentSize().width / 2 * 0.8f) - panel->getContentSize().width * 0.05f;
    float playY =
        panelBottom + (playSprite->getContentSize().height / 2 * 0.8f) + panel->getContentSize().height * 0.05f;

    // Tạo nút Play, ban đầu ẩn
    playItem = Utils::createButton("UI/buttonPlayGame.png", AX_CALLBACK_1(MapChooseScene::menuPlayCallback, this),
                                   Vec2(playX, playY), 0.8f, false);
    menuItems.pushBack(playItem);  // Thêm nút Play vào danh sách menu items

    // Tạo nút Buy với logic mua map, ban đầu ẩn
    auto buyButton = Utils::createButton("UI/buttonBuy.png", [this, panel](ax::Object* sender) {
        auto shopData    = ShopSystem::getInstance();
        auto gameData    = GameData::getInstance();
        int currentCoins = shopData->getCoins();
        int cost         = shopData->getCost("maps", selectedMapName);  // Lấy cost từ ShopSystem

        // Kiểm tra đủ điều kiện mua
        if (!selectedMapName.empty() && currentCoins >= cost)
        {
            shopData->setCoins(currentCoins - cost);                // Trừ coin
            shopData->setAvailable("maps", selectedMapName, true);  // Mở khóa trong shop
            gameData->setMapAvailable(selectedMapName, true);       // Mở khóa trong game
            shopData->syncMapsWithGameData();                       // Đồng bộ dữ liệu

            if (selectedMapItem)  // Nếu có map được chọn
            {
                // Cập nhật trạng thái available trong GameData
                auto& maps = const_cast<std::unordered_map<std::string, MapData>&>(gameData->getMaps());
                if (maps.find(selectedMapName) != maps.end())
                {
                    maps[selectedMapName].available = true;
                }

                // Cập nhật giao diện ngay lập tức
                Utils::updateItemUI(selectedMapItem, panel, true);  // Bỏ mờ map
                // Ẩn nhãn của map cũ (nếu có)
                if (auto* oldLabel = dynamic_cast<Label*>(selectedMapItem->getChildByName("label")))
                    oldLabel->setVisible(false);
                auto label = dynamic_cast<Label*>(selectedMapItem->getChildByName("label"));
                if (label)
                {
                    label->setString(selectedMapName);  // Cập nhật nhãn thành tên map
                    label->setVisible(true);            // Hiển thị nhãn
                }
                dynamic_cast<Node*>(sender)->setVisible(false);  // Ẩn nút Buy
                playItem->setVisible(true);                      // Hiện nút Play
            }

            // Cập nhật số coin hiển thị
            if (auto* coinLabel = dynamic_cast<Label*>(this->getChildByName("coinLabel")))
                coinLabel->setString(StringUtils::format("Coins: %d", shopData->getCoins()));

            // Lưu vào file save
            shopData->saveToFile(ax::FileUtils::getInstance()->getWritablePath() + "savegame.json");

            AXLOG("Unlocked map: %s", selectedMapName.c_str());
        }
        else
        {
            AXLOG("Not enough coins or no map selected!");
            dynamic_cast<Node*>(sender)->setVisible(false);  // Ẩn nút Buy nếu thất bại
        }
    }, Vec2(playX, playY), 0.8f, false);
    menuItems.pushBack(buyButton);  // Thêm nút Buy vào danh sách menu items

    // Lấy dữ liệu map từ GameData
    auto gameData      = GameData::getInstance();
    const auto& maps   = gameData->getMaps();
    int numMaps        = maps.size();                            // Số lượng map
    const int cols     = 1;                                      // Số cột hiển thị map
    float iconSpacingX = panel->getContentSize().width * 0.12f;  // Khoảng cách ngang giữa các button

    // Tính vị trí ban đầu cho các button map
    float panelLeft = panel->getPositionX() - panel->getContentSize().width / 2;
    float startX    = panel->getPositionX();
    float startY    = panel->getPositionY() + panel->getContentSize().height / 3;
    float yPos      = 100;  // Khoảng cách dọc ban đầu
    int index       = 0;    // Chỉ số để đặt vị trí button

    // Tạo các button map
    for (const auto& pair : maps)
    {
        const auto& map = pair.second;
        auto button =
            Utils::createMapButton(map.sprite, map.name, map.available, selectedMapName, selectedMapItem, playItem);

        // Tạo bản sao của map.name để capture vào lambda
        std::string mapName = map.name;
        button->setCallback([this, mapName, buyButton](ax::Object* sender) {
            selectedMapName = mapName;  // Sử dụng bản sao
            // Ẩn nhãn của map cũ trước khi chọn map mới
            if (selectedMapItem && selectedMapItem != sender)
                if (auto* oldLabel = dynamic_cast<Label*>(selectedMapItem->getChildByName("label")))
                    oldLabel->setVisible(false);
            if (selectedMapItem)
                selectedMapItem->setEnabled(true);                    // Bật lại button cũ
            selectedMapItem = dynamic_cast<MenuItemSprite*>(sender);  // Chọn button mới
            selectedMapItem->setEnabled(false);                       // Vô hiệu hóa button đang chọn

            // Cập nhật giao diện dựa trên trạng thái available từ GameData
            auto gameData    = GameData::getInstance();
            auto shopData    = ShopSystem::getInstance();
            bool isAvailable = gameData->getMaps().at(mapName).available;

            // Cập nhật giao diện button
            Utils::updateItemUI(selectedMapItem, nullptr, isAvailable);  // Cập nhật opacity (không cần panel)
            auto label = dynamic_cast<Label*>(selectedMapItem->getChildByName("label"));
            if (label)
            {
                if (isAvailable)
                {
                    label->setString(mapName);  // Hiển thị tên map nếu đã unlock
                }
                else
                {
                    int cost = shopData->getCost("maps", mapName);                    // Lấy cost từ ShopSystem
                    label->setString(StringUtils::format("Locked\nCost: %d", cost));  // Hiển thị "Locked" và cost
                }
                label->setVisible(true);  // Hiển thị nhãn
            }
            buyButton->setVisible(!isAvailable);  // Hiện nút Buy nếu chưa unlock
            playItem->setVisible(isAvailable);    // Hiện nút Play nếu đã unlock
        });

        // Đặt vị trí button theo công thức gốc
        float buttonY = startY + (button->getContentSize().height / 2) - yPos -
                        (index * (button->getContentSize().height * 0.4 + yPos));

        // Đặt vị trí button theo cột dọc
        button->setPosition(Vec2(startX, buttonY));
        menuItems.pushBack(button);  // Thêm button vào danh sách menu items
        index++;
        yPos += 25;  // Tăng khoảng cách dọc cho button tiếp theo
    }

    // Tạo menu từ danh sách menu items và thêm vào scene
    auto menu = Menu::createWithArray(menuItems);
    menu->setPosition(Vec2::ZERO);  // Đặt menu ở gốc tọa độ
    this->addChild(menu, 3);        // Thêm với z-order 3 (trên panel)
}
