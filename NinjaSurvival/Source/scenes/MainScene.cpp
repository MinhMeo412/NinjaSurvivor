#include "Utils.h"
#include "MainScene.h"
#include "CharacterChooseScene.h"
#include "ShopScene.h"
#include "AudioManager.h"
#include "systems/ShopSystem.h"

using namespace ax;

//// Định nghĩa các phím sẽ sử dụng
// static const ax::EventKeyboard::KeyCode keyMap[] = {
//     EventKeyboard::KeyCode::KEY_1, EventKeyboard::KeyCode::KEY_2, EventKeyboard::KeyCode::KEY_3,
//     EventKeyboard::KeyCode::KEY_4, EventKeyboard::KeyCode::KEY_Q, EventKeyboard::KeyCode::KEY_W,
//     EventKeyboard::KeyCode::KEY_E, EventKeyboard::KeyCode::KEY_R, EventKeyboard::KeyCode::KEY_A,
//     EventKeyboard::KeyCode::KEY_S, EventKeyboard::KeyCode::KEY_D, EventKeyboard::KeyCode::KEY_F};
//// Định nghĩa tên phím hiển thị
// static const std::string keyNames[] = {"Key 1", "Key 2", "Key 3", "Key 4", "Key Q", "Key W",
//                                        "Key E", "Key R", "Key A", "Key S", "Key D", "Key F"};
//// Hàm xử lý âm thanh tùy chỉnh cho từng phím
// void playCustomSound(EventKeyboard::KeyCode keyCode)
//{
//     switch (keyCode)
//     {
//     case EventKeyboard::KeyCode::KEY_1:
//         AudioManager::getInstance()->playSound("game_over", false, 1.5f, "music");
//         break;
//     case EventKeyboard::KeyCode::KEY_2:
//         AudioManager::getInstance()->playSound("exp", false, 1.0f, "item");
//         break;
//     case EventKeyboard::KeyCode::KEY_3:
//         AudioManager::getInstance()->playSound("bomb", false, 1.0f, "item");
//         break;
//     case EventKeyboard::KeyCode::KEY_4:
//         AudioManager::getInstance()->playSound("coin", false, 0.5f, "item");
//         break;
//     case EventKeyboard::KeyCode::KEY_Q:
//         AudioManager::getInstance()->playSound("magnet_heart", false, 1.0f, "item");
//         break;
//     case EventKeyboard::KeyCode::KEY_W:
//         AudioManager::getInstance()->playSound("chest", false, 1.0f, "item");
//         break;
//     case EventKeyboard::KeyCode::KEY_E:
//         AudioManager::getInstance()->playSound("get_hit", false, 1.0f, "effect");
//         break;
//     case EventKeyboard::KeyCode::KEY_R:
//         AudioManager::getInstance()->playSound("hit", false, 1.5f, "effect");
//         break;
//     case EventKeyboard::KeyCode::KEY_A:
//         AudioManager::getInstance()->playSound("sword", false, 1.0f, "effect");
//         break;
//     case EventKeyboard::KeyCode::KEY_S:
//         AudioManager::getInstance()->playSound("explosion_kunai", false, 1.0f, "effect");
//         break;
//     case EventKeyboard::KeyCode::KEY_D:
//         AudioManager::getInstance()->playSound("lightning_scroll", false, 1.0f, "effect");
//         break;
//     case EventKeyboard::KeyCode::KEY_F:
//         AudioManager::getInstance()->playSound("boss_alert", false, 1.0f, "music");
//         break;
//     default:
//         break;
//     }
// }

MainScene::MainScene() : SceneBase("MainScene") {}

bool MainScene::init()
{
    if (!SceneBase::init())
    {
        return false;
    }

    AudioManager::getInstance()->playSound("background_music", true, 0.5f, "background");

    float itemSpacing = 50.0;
    int itemOrderNum  = 0;
    float centerX     = safeOrigin.x + visibleSize.width / 2;
    float centerY     = safeOrigin.y + visibleSize.height / 2;

    // Background image (need update)

    auto background = Sprite::create("UI/background2.png");
    background->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));
    this->addChild(background, 0);

    auto titleSprite = Sprite::create("UI/NeoTitle1.png");
    titleSprite->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height * 5 / 6));
    this->addChild(titleSprite, 1);

    float buttonSpacing = visibleSize.width / 10;  // Khoảng cách đều nhau giữa các nút
    float startY        = origin.y + visibleSize.height * 3 / 4 - visibleSize.height / 6;
    // Play button
    playItem = Utils::createMenuItem("UI/buttonPlayGame.png", "UI/buttonPlayGame.png",
                                     AX_CALLBACK_1(MainScene::menuPlayCallback, this),
                                     Vec2(origin.x + visibleSize.width / 2, startY));

    shopItem = Utils::createMenuItem("UI/buttonShop.png", "UI/buttonShop.png",
                                     AX_CALLBACK_1(MainScene::menuShopCallback, this),
                                     Vec2(origin.x + visibleSize.width / 2, startY - 100));

    closeItem = Utils::createMenuItem("UI/buttonQuit.png", "UI/buttonQuit.png",
                                      AX_CALLBACK_1(MainScene::menuCloseCallback, this),
                                      Vec2(origin.x + visibleSize.width / 2, startY - 200));

    auto menu = Menu::create(playItem, shopItem, closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    //// Tạo label để hiển thị phím được nhấn
    // auto label = Label::createWithTTF("Press a key...", "fonts/Marker Felt.ttf", 24);
    // label->setPosition(Vec2(100,100));
    // this->addChild(label, 1);
    //// Tạo keyboard listener
    // auto keyboardListener          = EventListenerKeyboard::create();
    // keyboardListener->onKeyPressed = [label](EventKeyboard::KeyCode keyCode, Event* event) {
    //     for (size_t i = 0; i < sizeof(keyMap) / sizeof(keyMap[0]); ++i)
    //     {
    //         if (keyCode == keyMap[i])
    //         {
    //             std::string keyPressed = "Pressed: " + keyNames[i];
    //             label->setString(keyPressed);
    //             AXLOG("Key pressed: %s", keyNames[i].c_str());  // In ra console
    //             playCustomSound(keyCode);                       // Gọi hàm xử lý âm thanh tùy chỉnh
    //             break;
    //         }
    //     }
    // };
    //  Đăng ký listener
    //_eventDispatcher->addEventListenerWithSceneGraphPriority(keyboardListener, this);

    return true;
}

void MainScene::update(float dt) {}

void MainScene::menuCloseCallback(ax::Object* sender)
{
    // âm thanh click
    AudioManager::getInstance()->playSound("button_click", false, 1.0f, "click");

    _director->end();
}

void MainScene::menuPlayCallback(ax::Object* sender)
{
    // âm thanh click
    AudioManager::getInstance()->playSound("button_click", false, 1.0f, "click");

    auto scene = utils::createInstance<CharacterChooseScene>();

    _director->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}

void MainScene::menuShopCallback(ax::Object* sender)
{
    // âm thanh click
    AudioManager::getInstance()->playSound("button_click", false, 1.0f, "click");

    auto scene = utils::createInstance<ShopScene>();

    _director->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}
