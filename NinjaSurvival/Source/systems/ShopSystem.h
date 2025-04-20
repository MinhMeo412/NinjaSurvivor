#ifndef __SHOP_SYSTEM_H__
#define __SHOP_SYSTEM_H__

#include "axmol.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <optional>

struct ShopData
{
    std::string type;
    std::string name;
    bool available;
    std::optional<int> level;
    std::optional<float> levelValue;
    std::optional<int> cost;
    std::optional<int> levelLimit;
    std::optional<float> valueIncrease;
    std::optional<float> valueIncrement;
};

class ShopSystem
{
public:
    ShopSystem();
    ~ShopSystem();

    static ShopSystem* getInstance();
    static ShopSystem* createInstance();

    bool createSaveGame();
    bool loadSaveGame();
    bool saveToFile(const std::string& filename);
    bool isSaveGameExist() const;

    int getCoins() const;
    void setCoins(int value);
    const std::vector<ShopData>& getShopData() const { return shopData; }
    void setAvailable(const std::string& type, const std::string& name, bool available);

    int getCost(const std::string& type, const std::string& name) const;
    int getLevel(const std::string& type, const std::string& name) const;
    int getLevelLimit(const std::string& type, const std::string& name) const;
    float getValueIncrease(const std::string& type, const std::string& name) const;
    float getValueIncrement(const std::string& type, const std::string& name) const;
    float getStatLevelValue(const std::string& type, const std::string& name) const;
    float getShopBuff(const std::string& buffName) const;

    bool upgradeStat(const std::string& name);

    void syncCharactersWithGameData();
    void syncMapsWithGameData();
    void syncCoinsWithGameData(float coinMultiplier);
    void syncRerollCountWithLevelSystem();

    int getShopDataVersion() const { return shopDataVersion; }

private:
    static std::unique_ptr<ShopSystem> instance;
    std::vector<ShopData> shopData;
    int shopDataVersion = 0;  // Theo dõi thay đổi dữ liệu
    std::string readFileContent(const std::string& filename);
    std::optional<int> pendingRerollCount;
};

#endif
