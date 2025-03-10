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

//Check null ptr before set visible
void setVisibleSafe(ax::Node* node, bool visible);
}  // namespace Utils

#endif  // __UTILS_H__
