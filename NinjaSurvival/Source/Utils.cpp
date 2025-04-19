#include "Utils.h"

namespace Utils
{
// Print useful error message instead of segfaulting when files are not there.
void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
}

// Create menu items using sprite sheet
ax::MenuItemImage* createMenuItem(ax::Sprite* normalImage,
                                  ax::Sprite* selectedImage,
                                  const ax::ccMenuCallback& callback,
                                  const ax::Vec2& position)

{
    auto menuItem = ax::MenuItemImage::create();
    if (!menuItem)
    {
        problemLoading("MenuItemImage creation failed");
        return nullptr;
    }

    // Check normalImage
    if (!normalImage || normalImage->getContentSize().width <= 0 || normalImage->getContentSize().height <= 0)
    {
        problemLoading("normalImage sprite");
        delete menuItem;
        return nullptr;
    }

    // Check selectedImage
    if (!selectedImage || selectedImage->getContentSize().width <= 0 || selectedImage->getContentSize().height <= 0)
    {
        problemLoading("selectedImage sprite");
        delete menuItem;
        return nullptr;
    }

    menuItem->setNormalImage(normalImage);
    menuItem->setSelectedImage(selectedImage);
    menuItem->setCallback(callback);
    menuItem->setPosition(position);

    return menuItem;
}

// Create menu items using single sprite
ax::MenuItemImage* createMenuItem(std::string normalImage,
                                  std::string selectedImage,
                                  const ax::ccMenuCallback& callback,
                                  const ax::Vec2& position)
{
    auto menuItem = ax::MenuItemImage::create(normalImage, selectedImage, callback);
    if (!menuItem)
    {
        problemLoading("MenuItemImage creation failed");
        return nullptr;
    }

    if (menuItem == nullptr || menuItem->getContentSize().width <= 0 || menuItem->getContentSize().height <= 0)
    {
        problemLoading((normalImage + " and " + selectedImage).c_str());
        delete menuItem;
        return nullptr;
    }

    menuItem->setPosition(position);
    return menuItem;
}

// Character button in CharacterChooseScene
ax::MenuItemSprite* createCharacterButton(const std::string& spritePath,
                                          const std::string& buttonName,
                                          bool available,
                                          std::string& selectedCharacterName,
                                          ax::MenuItemSprite*& selectedCharacterItem,
                                          ax::MenuItemSprite* playButton)
{
    auto normalSprite   = ax::Sprite::create(spritePath);
    auto selectedSprite = ax::Sprite::create(spritePath);

    auto characterItem = ax::MenuItemSprite::create(normalSprite, selectedSprite, nullptr);

    // std::string displayName = available ? buttonName : "Locked";
    // auto characterLabel           = ax::Label::createWithTTF(displayName, "fonts/Marker Felt.ttf", 24);
    // ax::Vec2 itemPos        = characterItem->getPosition();  // Vị trí của characterItem trên màn hình
    // ax::Vec2 relativePos    = namePos - itemPos;
    // characterLabel->setPosition(relativePos);
    // characterLabel->setVisible(false);

    // characterItem->addChild(characterLabel, 6, "label");

    auto drawNode = ax::DrawNode::create();
    drawNode->drawRect(ax::Vec2(0, 0),
                       ax::Vec2(characterItem->getContentSize().width, characterItem->getContentSize().height),
                       ax::Color4F::GREEN, 1.5f);
    drawNode->setVisible(false);
    characterItem->addChild(drawNode, 3, "border");

    characterItem->setCallback([characterItem, &selectedCharacterItem, buttonName, available, &selectedCharacterName,
                                playButton](ax::Object* sender) {
        if (selectedCharacterItem && selectedCharacterItem != characterItem)
        {
            setVisibleSafe(selectedCharacterItem->getChildByName("border"), false);
            if (selectedCharacterItem->getUserData())
            {
                auto oldLabel = static_cast<ax::Label*>(selectedCharacterItem->getUserData());
                setVisibleSafe(oldLabel, false);
            }
        }

        selectedCharacterItem = characterItem;

        setVisibleSafe(characterItem->getChildByName("border"), true);

        if (characterItem->getUserData())
        {
            auto label = static_cast<ax::Label*>(characterItem->getUserData());
            setVisibleSafe(label, true);
        }
        selectedCharacterName = buttonName;

        if (playButton)
        {
            playButton->setVisible(available);
        }
    });

    if (!available)
    {
        characterItem->setOpacity(128);
    }

    return characterItem;
}

// Map button in MapChooseScene
ax::MenuItemSprite* createMapButton(const std::string& spritePath,
                                    const std::string& buttonName,
                                    bool available,
                                    std::string& selectedMapName,
                                    ax::MenuItemSprite*& selectedMapItem,
                                    ax::MenuItemSprite* playButton)
{
    auto normalSprite   = ax::Sprite::create(spritePath);
    auto selectedSprite = ax::Sprite::create(spritePath);

    auto mapItem = ax::MenuItemSprite::create(normalSprite, selectedSprite, nullptr);

    std::string displayName = available ? buttonName : "Locked";
    auto mapLabel           = ax::Label::createWithTTF(displayName, "fonts/Pixelpurl-0vBPP.ttf", 35);
    mapLabel->setPosition(normalSprite->getContentSize().width / 2, normalSprite->getContentSize().height + (mapLabel->getContentSize().height / 2));
    mapLabel->setColor(ax::Color3B::BLACK);
    // mapLabel->setScale(3);
    mapLabel->setVisible(false);
    mapItem->addChild(mapLabel, 3, "label");

    auto drawNode = ax::DrawNode::create();
    drawNode->drawRect(ax::Vec2(0, 0), ax::Vec2(mapItem->getContentSize().width, mapItem->getContentSize().height),
                       ax::Color4F::GREEN, 1.5f);
    drawNode->setVisible(false);
    mapItem->addChild(drawNode, 3, "border");

    mapItem->setCallback(
        [mapItem, &selectedMapItem, buttonName, available, &selectedMapName, playButton](ax::Object* sender) {
        if (selectedMapItem && selectedMapItem != mapItem)
        {
            setVisibleSafe(selectedMapItem->getChildByName("border"), false);
            setVisibleSafe(selectedMapItem->getChildByName("label"), false);
        }

        selectedMapItem = mapItem;

        setVisibleSafe(mapItem->getChildByName("border"), true);
        setVisibleSafe(mapItem->getChildByName("label"), true);

        selectedMapName = buttonName;

        if (playButton)
        {
            playButton->setVisible(available);
        }
    });

    if (!available)
    {
        mapItem->setOpacity(128);
    }

    return mapItem;
}
ax::MenuItemSprite* Utils::createStatButton(const std::string& spritePath, const std::string& statValue)
{
    auto normalSprite = ax::Sprite::create(spritePath);
    if (!normalSprite)
    {
        AXLOG("Failed to load sprite: %s, using default", spritePath.c_str());
        normalSprite = ax::Sprite::create("CloseNormal.png");
    }
    auto selectedSprite = ax::Sprite::create(spritePath);
    if (!selectedSprite)
        selectedSprite = ax::Sprite::create("CloseNormal.png");

    auto statItem = ax::MenuItemSprite::create(normalSprite, selectedSprite, nullptr);

    // Tạo viền cho stat button
    auto drawNode = ax::DrawNode::create();
    drawNode->drawRect(ax::Vec2(0, 0), ax::Vec2(statItem->getContentSize().width, statItem->getContentSize().height),
                       ax::Color4F::GREEN, 1.5f);
    drawNode->setVisible(false);
    statItem->addChild(drawNode, 3, "border");

    return statItem;
}

ax::MenuItemSprite* createButton(const std::string& spritePath,
                                 const ax::ccMenuCallback& callback,
                                 const ax::Vec2& pos,
                                 float scale,
                                 bool visible)
{
    auto sprite         = ax::Sprite::create(spritePath);
    auto selectedSprite = ax::Sprite::create(spritePath);
    auto button         = ax::MenuItemSprite::create(sprite, selectedSprite, nullptr, callback);
    button->setPosition(pos);
    button->setScale(scale);
    button->setVisible(visible);
    return button;
}

void Utils::updateItemUI(ax::MenuItemSprite* item, ax::Node* panel, bool isAvailable)
{
    item->setOpacity(isAvailable ? 255 : 128);
}

// Check null ptr before set visible
void setVisibleSafe(ax::Node* node, bool visible)
{
    if (node)
        node->setVisible(visible);
}

float getRandomFloat(float min, float max)
{
    static std::mt19937 rng{std::random_device{}()};  // static để giữ seed 1 lần duy nhất
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

int getRandomInt(int min, int max)
{
    static std::mt19937 rng{std::random_device{}()};
    std::uniform_int_distribution<int> dist(min, max);
    return dist(rng);
}

}  // namespace Utils
