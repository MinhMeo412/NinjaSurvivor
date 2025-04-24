#ifndef __UTILS_H__
#define __UTILS_H__

#include "axmol.h"

#define DISPLAY_TIME_SPLASH_SCENE 1.0
#define TRANSITION_TIME           0.5
#define DELAY_LOADING_SCENE       3.0

namespace Utils
{
// Print useful error message instead of segfaulting when files are not there.
void problemLoading(const char* filename);

// Create menu items
ax::MenuItemImage* createMenuItem(ax::Sprite* normalImage,
                                  ax::Sprite* selectedImage,
                                  const ax::ccMenuCallback& callback,
                                  const ax::Vec2& position);

// Create menu items
ax::MenuItemImage* createMenuItem(std::string normalImage,
                                  std::string selectedImage,
                                  const ax::ccMenuCallback& callback,
                                  const ax::Vec2& position);

// Create map button
ax::MenuItemSprite* createMapButton(const std::string& spritePath,
                                    const std::string& buttonName,
                                    bool available,
                                    std::string& selectedMapName,
                                    ax::MenuItemSprite*& selectedMapItem,
                                    ax::MenuItemSprite* playButton);

ax::MenuItemSprite* createCharacterButton(const std::string& spritePath,
                                          const std::string& buttonName,
                                          bool available,
                                          std::string& selectedCharacterName,
                                          ax::MenuItemSprite*& selectedCharacterItem,
                                          ax::MenuItemSprite* playButton);

ax::MenuItemSprite* createButton(const std::string& spritePath,
                                 const ax::ccMenuCallback& callback,
                                 const ax::Vec2& position,
                                 float scale,
                                 bool visible);

ax::MenuItemSprite* createStatButton(const std::string& spritePath, const std::string& statValue);

void updateItemUI(ax::MenuItemSprite* item, ax::Node* panel, bool isAvailable);
// Check null ptr before set visible
void setVisibleSafe(ax::Node* node, bool visible);

// Check nếu giá trị không nằm trong chuỗi (cùng kiểu dữ liệu)
template <typename T, typename... Args>
bool not_in(const T& value, Args... args)
{
    return ((value != args) && ...);
}

// Check nếu giá trị nằm trong chuỗi (cùng kiểu dữ liệu)
template <typename T, typename... Args>
bool in(const T& value, Args... args)
{
    return ((value == args) || ...);
}

// Lấy giá trị ngẫu nhiên
float getRandomFloat(float min, float max);
int getRandomInt(int min, int max);

}  // namespace Utils

#endif  // __UTILS_H__
