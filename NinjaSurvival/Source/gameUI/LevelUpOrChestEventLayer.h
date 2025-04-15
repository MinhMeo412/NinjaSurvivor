#ifndef __LEVELUP_OR_CHEST_EVENT_LAYER_H__
#define __LEVELUP_OR_CHEST_EVENT_LAYER_H__

#include "axmol.h"


class LevelUpOrChestEventLayer : public ax::Layer
{
public:
    static LevelUpOrChestEventLayer* create(bool isLevelUp,
                                            const std::vector<std::pair<std::string,int>>& upgradeList);

    LevelUpOrChestEventLayer(bool isLevelUp, const std::vector<std::pair<std::string, int>>& upgradeList);

    bool init() override;


private:
    bool isLevelUp;
    std::vector<std::pair<std::string, int>> upgradeList;
    
    ax::Menu* selectionMenu;
    std::string selectedUpgrade;

    int rerollCount = 0;

    ax::MenuItemImage* confirmButton = nullptr;
    ax::MenuItemImage* rerollButton  = nullptr;

    ax::Label* rerollCountLabel;

    void createUI();
    void onConfirm(ax::Object* sender);
    void onReroll(ax::Object* sender);
};

#endif  // __LEVELUP_OR_CHEST_EVENT_LAYER_H__
