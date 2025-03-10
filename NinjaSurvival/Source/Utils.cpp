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

    std::string displayName = available ? buttonName : "Locked";
    auto characterLabel           = ax::Label::createWithTTF(displayName, "fonts/Marker Felt.ttf", 24);
    characterLabel->setPosition(normalSprite->getContentSize().width / 2, normalSprite->getContentSize().height / 2);
    characterLabel->setVisible(false);
    characterItem->addChild(characterLabel, 3, "label");

    auto drawNode = ax::DrawNode::create();
    drawNode->drawRect(ax::Vec2(0, 0),
                       ax::Vec2(characterItem->getContentSize().width, characterItem->getContentSize().height),
                       ax::Color4F::RED);
    drawNode->setVisible(false);
    characterItem->addChild(drawNode, 3, "border");

    characterItem->setCallback([characterItem, &selectedCharacterItem, buttonName, available, &selectedCharacterName,
                                playButton](ax::Object* sender) {
        if (selectedCharacterItem && selectedCharacterItem != characterItem)
        {
            setVisibleSafe(selectedCharacterItem->getChildByName("border"), false);
            setVisibleSafe(selectedCharacterItem->getChildByName("label"), false);
        }

        selectedCharacterItem = characterItem;

        setVisibleSafe(characterItem->getChildByName("border"), true);
        setVisibleSafe(characterItem->getChildByName("label"), true);

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
    auto mapLabel           = ax::Label::createWithTTF(displayName, "fonts/Marker Felt.ttf", 24);
    mapLabel->setPosition(normalSprite->getContentSize().width / 2, normalSprite->getContentSize().height / 2);
    mapLabel->setVisible(false);
    mapItem->addChild(mapLabel, 3, "label");

    auto drawNode = ax::DrawNode::create();
    drawNode->drawRect(ax::Vec2(0, 0), ax::Vec2(mapItem->getContentSize().width, mapItem->getContentSize().height),
                       ax::Color4F::RED);
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

// Check null ptr before set visible
void setVisibleSafe(ax::Node* node, bool visible)
{
    if (node)
        node->setVisible(visible);
}


}  // namespace Utils
