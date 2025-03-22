#include "Utils.h"
#include "MainScene.h"
#include "CharacterChooseScene.h"

#include "systems/GameData.h"

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

    // Play button
    playItem =
        Utils::createMenuItem("CloseNormal.png", "CloseSelected.png", AX_CALLBACK_1(MainScene::menuPlayCallback, this),
                              Vec2(centerX, centerY - itemOrderNum++ * itemSpacing));

    auto label1 = Label::createWithTTF("Play button", "fonts/Marker Felt.ttf", 24);
    label1->setPosition(playItem->getPosition() + Vec2(100, 0));
    this->addChild(label1, 1);

    // Close button
    closeItem =
        Utils::createMenuItem("CloseNormal.png", "CloseSelected.png", AX_CALLBACK_1(MainScene::menuCloseCallback, this),
                              Vec2(centerX, centerY - itemOrderNum++ * itemSpacing));

    auto label2 = Label::createWithTTF("Exit button", "fonts/Marker Felt.ttf", 24);
    label2->setPosition(closeItem->getPosition() + Vec2(100, 0));
    this->addChild(label2, 1);

    // Create menu
    auto menu = Menu::create(playItem, closeItem, NULL);
    menu->setPosition(Vec2::ZERO);
    this->addChild(menu, 1);

    // Title (replace with image)
    auto label = Label::createWithTTF("Ninja Survival Title", "fonts/Marker Felt.ttf", 24);
    label->setPosition(
        Vec2(origin.x + visibleSize.width / 2, origin.y + visibleSize.height - label->getContentSize().height));
    this->addChild(label, 1);

    // Border
    auto drawNode = DrawNode::create();
    drawNode->setPosition(Vec2(0, 0));
    addChild(drawNode);
    drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4F::BLUE);



        // Kiểm tra đọc file entities.json (xóa khi release)
    auto gameData               = GameData::getInstance();
    const auto& entityTemplates = gameData->getEntityTemplates();

    if (!entityTemplates.empty())
    {
        AXLOG("Entity Templates:");
        for (const auto& [category, entities] : entityTemplates)
        {
            AXLOG("  Category: %s", category.c_str());
            for (const auto& [key, entity] : entities)
            {
                AXLOG("    Entity Key: %s", key.c_str());
                AXLOG("      Type: %s", entity.type.c_str());
                AXLOG("      Name: %s", entity.name.c_str());
                AXLOG("      Available: %s", entity.available ? "true" : "false");

                if (entity.transform)
                {
                    AXLOG("      Transform Component: Exists");
                }
                if (entity.sprite)
                {
                    const auto& spriteComp = entity.sprite.value();
                    AXLOG("      Sprite Component: Exists");
                    AXLOG("        Plist: %s", spriteComp.plist.c_str());
                    AXLOG("        Frame Name: %s", spriteComp.gameSceneFrameName.c_str());
                    AXLOG("      Sprite Component: Exists");
                }
                if (entity.animation)
                {
                    AXLOG("      Animation Component: Exists");
                }
                if (entity.velocity)
                {
                    AXLOG("      Velocity Component: Exists");
                }
            }
        }
    }
    else
    {
        AXLOG("Entity Templates is empty.");
    }

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
