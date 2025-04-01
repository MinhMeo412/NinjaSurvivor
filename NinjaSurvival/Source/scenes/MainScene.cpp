#include "Utils.h"
#include "MainScene.h"
#include "CharacterChooseScene.h"

using namespace ax;

MainScene::MainScene() : SceneBase("MainScene") {}

bool MainScene::init()
{
    if (!SceneBase::init())
    {
        return false;
    }

    float itemSpacing = 50.0;
    int itemOrderNum  = 0;
    float centerX     = safeOrigin.x + visibleSize.width / 2;
    float centerY     = safeOrigin.y + visibleSize.height / 2;

    // Background image (need update)

    auto background = Sprite::create("UI/background2.png");
    background->setPosition(Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height / 2));

    // Điều chỉnh tỷ lệ để phủ kín màn hình
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

    /*
    Chưa có scene shop vs archive thì chưa cần đến
    auto shopButton =
        MenuItemImage::create("UI/buttonShop.png", "UI/buttonShop.png", AX_CALLBACK_1(MainScene::shopCallback, this));
    shopButton->setPosition(
        Vec2(origin.x + visibleSize.width / 2, startY - buttonSpacing - shopButton->getContentSize().height));


    auto achieveButton = MenuItemImage::create("UI/buttonArchive.png", "UI/buttonArchive.png",
                                               AX_CALLBACK_1(MainScene::achieveCallback, this));
    achieveButton->setPosition(Vec2(
        origin.x + visibleSize.width / 2,
       startY - 2 * buttonSpacing - shopButton->getContentSize().height - achieveButton->getContentSize().height));
    */
    auto menu = Menu::create(playItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // Border
    auto drawNode = DrawNode::create();
    drawNode->setPosition(Vec2(0, 0));
    addChild(drawNode);
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4F::BLUE);

    return true;
}

void MainScene::update(float dt) {}

void MainScene::menuCloseCallback(ax::Object* sender)
{
    _director->end();
}

void MainScene::menuPlayCallback(ax::Object* sender)
{
    auto scene = utils::createInstance<CharacterChooseScene>();

    _director->replaceScene(TransitionFade::create(TRANSITION_TIME, scene));
}
