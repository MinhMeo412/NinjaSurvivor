#ifndef __SHOP_SYSTEM_H__
#define __SHOP_SYSTEM_H__

#include "System.h"
#include "components/ComponentManager.h"
#include "entities/EntityManager.h"
#include "axmol.h"
#include <string>
#include <memory> 
#include <unordered_map>
#include <vector>
#include <optional>

struct ShopData
{
    std::string type;               // Loại dữ liệu: "Character", "Maps", "Stat", "Coin", v.v.
    std::string name;               // Tên (dùng cho Character, Maps, Stat; không dùng cho Coin)
    bool available;                 // Trạng thái unlock (chỉ dùng cho Character và Maps)
    std::optional<int> level;       // Cấp độ (chỉ dùng cho Stat)
    std::optional<int> levelValue;  // Giá trị (dùng cho Stat và Coin)
    std::optional<int> cost;        // Giá trị cần trả để mua hoặc nâng cấp
    std::optional<int> levelLimit;  // Giới hạn cấp độ tối đa (chỉ dùng cho Stat)
    std::optional<float>
        valueIncrease;  // Giá trị tăng cho levelValue mỗi lần nâng cấp (có thể là phần trăm hoặc giá trị cố định)
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

    // Getter và Setter
    int getCoins() const;  // Lấy số coin từ dữ liệu Coin
    void setCoins(int value);
    const std::vector<ShopData>& getShopData() const { return shopData; }
    void setStatLevel(const std::string& name, int level, int levelValue, float valueIncrease);
    void setAvailable(const std::string& type, const std::string& name, bool available);

    // Getter cho các biến mới
    int getCost(const std::string& type, const std::string& name) const;
    int getLevelLimit(const std::string& type, const std::string& name) const;
    float getValueIncrease(const std::string& type, const std::string& name) const;

    // Hàm đồng bộ mới
    void syncCharactersWithGameData();                 // Đồng bộ cho Character
    void syncMapsWithGameData();                       // Đồng bộ cho Maps
    void syncCoinsWithGameData(float coinMultiplier);  // Đồng bộ cho Coin với hệ số

private:

    static std::unique_ptr<ShopSystem> instance;

    std::vector<ShopData> shopData;  // Danh sách dữ liệu tổng quát

    std::string readFileContent(const std::string& filename);
};
#endif
