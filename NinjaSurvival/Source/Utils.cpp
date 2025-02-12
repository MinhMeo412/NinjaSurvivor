#include "Utils.h"

namespace Utils
{
// Print useful error message instead of segfaulting when files are not there.
void problemLoading(const char* filename)
{
    printf("Error while loading: %s\n", filename);
}

//Create menu items using sprite sheet
ax::MenuItemImage* createMenuItem(ax::Sprite* normalImage,
                                  ax::Sprite* selectedImage,
                                         const ax::ccMenuCallback& callback,
                                         const ax::Vec2& position)

{
    auto menuItem = ax::MenuItemImage::create();
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

    if (menuItem == nullptr || menuItem->getContentSize().width <= 0 || menuItem->getContentSize().height <= 0)
    {
        problemLoading((normalImage + " and " + selectedImage).c_str());
        return nullptr;
    }
    menuItem->setPosition(position);
    return menuItem;
}
}  // namespace Utils
