#include "ShopSystem.h"
#include "GameData.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

using namespace rapidjson;

std::unique_ptr<ShopSystem> ShopSystem::instance = nullptr;

ShopSystem::ShopSystem() {}

ShopSystem::~ShopSystem() {}

ShopSystem* ShopSystem::createInstance()
{
    return new ShopSystem();
}

ShopSystem* ShopSystem::getInstance()
{
    static bool statsLoaded = false;
    if (!instance)
    {
        AXLOG("Khởi tạo instance ShopSystem");
        instance.reset(createInstance());
        if (!statsLoaded)
        {
            instance->createSaveGame();    // Tạo file mặc định nếu chưa có
            if (instance->loadSaveGame())  // Tải dữ liệu từ savegame.json
            {
                statsLoaded = true;
                // Đồng bộ ngay sau khi khởi tạo
                GameData::getInstance()->syncStatsWithShopSystem();
            }
            else
            {
                AXLOG("Lỗi: Không thể tải savegame.json lúc khởi tạo");
            }
        }
    }
    else
    {
        AXLOG("Instance ShopSystem đã tồn tại");
    }
    return instance.get();
}

std::string ShopSystem::readFileContent(const std::string& filename)
{
    std::string fullPath = ax::FileUtils::getInstance()->fullPathForFilename(filename);
    if (fullPath.empty())
    {
        AXLOG("Lỗi: Không tìm thấy file %s", filename.c_str());
        return "";
    }
    std::string content = ax::FileUtils::getInstance()->getStringFromFile(fullPath);
    if (content.empty())
    {
        AXLOG("Lỗi: Không đọc được nội dung từ %s", filename.c_str());
    }
    else
    {
        AXLOG("Đọc thành công %s: %d bytes", filename.c_str(), content.size());
    }
    return content;
}

bool ShopSystem::createSaveGame()
{
    std::string filePath = ax::FileUtils::getInstance()->getWritablePath() + "savegame.json";

    if (isSaveGameExist())
    {
        AXLOG("File savegame đã tồn tại tại: %s", filePath.c_str());
        return true;
    }

    // Khởi tạo dữ liệu mặc định
    shopData = {
        {"Coin", "", false, std::nullopt, 1000, std::nullopt, std::nullopt, std::nullopt, std::nullopt},
        {"Stat", "Health", false, 0, 0, 50, 10, 0.1f, 10.0f},  // Health: +10 mỗi cấp
        {"Stat", "Attack", false, 0, 0, 50, 10, 0.1f, 5.0f},   // Attack: +5 mỗi cấp
        {"Stat", "Speed", false, 0, 0, 50, 10, 0.1f, 2.0f},    // Speed: +2 mỗi cấp
        {"Stat", "XPGain", false, 0, 0, 50, 5, 0.1f, 10.0f},          // XP Gain: +10% mỗi cấp (5 cấp)
        {"Stat", "CoinGain", false, 0, 0, 50, 10, 0.1f, 10.0f},       // Coin Gain: +10% mỗi cấp (10 cấp)
        {"Stat", "RerollWeapon", false, 0, 0, 50, 3, 0.0f, 1.0f},     // RerollWeapon: +1 lần mỗi cấp (3 cấp)
        {"Stat", "ReduceCooldown", false, 0, 0, 50, 5, 0.1f, 10.0f},  // Reduce Cooldown: -10% mỗi cấp (5 cấp)
        {"Stat", "SpawnRate", false, 0, 0, 50, 10, 0.1f, 10.0f},      // Spawn Rate: +10% mỗi cấp (10 cấp)
        {"entities", "Ninja", true, std::nullopt, std::nullopt, 100, std::nullopt, std::nullopt, std::nullopt},
        {"entities", "Master", false, std::nullopt, std::nullopt, 200, std::nullopt, std::nullopt, std::nullopt},
        {"maps", "Map", true, std::nullopt, std::nullopt, 0, std::nullopt, std::nullopt, std::nullopt},
        {"maps", "Large Map", false, std::nullopt, std::nullopt, 150, std::nullopt, std::nullopt, std::nullopt}};

    shopDataVersion++;
    return saveToFile(filePath);
}

bool ShopSystem::isSaveGameExist() const
{
    std::string filePath = ax::FileUtils::getInstance()->getWritablePath() + "savegame.json";
    return ax::FileUtils::getInstance()->isFileExist(filePath);
}

bool ShopSystem::saveToFile(const std::string& filename)
{
    Document doc;
    doc.SetObject();
    auto& allocator = doc.GetAllocator();

    Value dataArray(kArrayType);
    for (const auto& data : shopData)
    {
        Value dataObj(kObjectType);
        dataObj.AddMember("type", Value(data.type.c_str(), allocator), allocator);
        if (!data.name.empty())
            dataObj.AddMember("name", Value(data.name.c_str(), allocator), allocator);
        if (data.type == "entities" || data.type == "maps")
            dataObj.AddMember("available", data.available, allocator);
        if (data.type == "Stat" && data.level.has_value())
            dataObj.AddMember("level", data.level.value(), allocator);
        if ((data.type == "Stat" || data.type == "Coin") && data.levelValue.has_value())
            dataObj.AddMember("levelValue", data.levelValue.value(), allocator);
        if (data.cost.has_value())
            dataObj.AddMember("cost", data.cost.value(), allocator);
        if (data.levelLimit.has_value())
            dataObj.AddMember("levelLimit", data.levelLimit.value(), allocator);
        if (data.valueIncrease.has_value())
            dataObj.AddMember("valueIncrease", data.valueIncrease.value(), allocator);
        if (data.valueIncrement.has_value())
            dataObj.AddMember("valueIncrement", data.valueIncrement.value(), allocator);

        dataArray.PushBack(dataObj, allocator);
    }

    doc.AddMember("data", dataArray, allocator);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    std::string jsonData = buffer.GetString();

    if (ax::FileUtils::getInstance()->writeStringToFile(jsonData, filename))
    {
        AXLOG("Đã lưu vào %s: %s", filename.c_str(), jsonData.c_str());
        return true;
    }
    else
    {
        AXLOG("Lỗi khi lưu vào %s", filename.c_str());
        return false;
    }
}

bool ShopSystem::upgradeStat(const std::string& name)
{
    for (auto& data : shopData)
    {
        if (data.type == "Stat" && data.name == name)
        {
            // Kiểm tra dữ liệu hợp lệ
            if (!data.level.has_value() || !data.levelLimit.has_value() || !data.cost.has_value() ||
                !data.valueIncrement.has_value())
            {
                AXLOG("Dữ liệu chỉ số %s không hợp lệ", name.c_str());
                return false;
            }

            int currentLevel = data.level.value();
            if (currentLevel >= data.levelLimit.value())
            {
                AXLOG("Chỉ số %s đã đạt giới hạn cấp độ: %d", name.c_str(), data.levelLimit.value());
                return false;
            }

            int currentCoins = getCoins();
            if (currentCoins < data.cost.value())
            {
                AXLOG("Không đủ coin để nâng cấp %s", name.c_str());
                return false;
            }

            // Trừ coin
            setCoins(currentCoins - data.cost.value());

            // Tăng cấp độ
            data.level = currentLevel + 1;

            // Tính levelValue
            if (data.name == "RerollWeapon")
            {
                data.levelValue = data.level.value();  // RerollWeapon: levelValue = level
            }
            else if (data.name == "XPGain" || data.name == "CoinGain" || data.name == "ReduceCooldown" ||
                     data.name == "SpawnRate")
            {
                data.levelValue = static_cast<int>(data.level.value() * data.valueIncrement.value());
            }
            else
            {
                data.levelValue = static_cast<int>(data.level.value() * data.valueIncrement.value());
            }

            // Cập nhật cost (trừ RerollWeapon)
            if (data.name != "RerollWeapon")
            {
                int currentCost = data.cost.value();
                data.cost       = static_cast<int>(currentCost * (1.0f + data.valueIncrease.value()));
            }

            // Cập nhật cost
            int currentCost = data.cost.value();
            data.cost       = static_cast<int>(currentCost * (1.0f + data.valueIncrease.value()));

            // Lưu và đồng bộ
            shopDataVersion++;
            saveToFile(ax::FileUtils::getInstance()->getWritablePath() + "savegame.json");
            GameData::getInstance()->syncStatsWithShopSystem();

            AXLOG("Đã nâng cấp %s: level=%d, levelValue=%d, cost=%d", name.c_str(), data.level.value(),
                  data.levelValue.value(), data.cost.value());
            return true;
        }
    }
    AXLOG("Không tìm thấy chỉ số %s", name.c_str());
    return false;
}

bool ShopSystem::loadSaveGame()
{
    std::string filename   = ax::FileUtils::getInstance()->getWritablePath() + "savegame.json";
    std::string jsonString = readFileContent(filename);

    if (jsonString.empty())
    {
        AXLOG("Không thể đọc savegame.json, tạo file mới");
        if (!createSaveGame())
        {
            AXLOG("Lỗi: Không thể tạo savegame.json");
            return false;
        }
        jsonString = readFileContent(filename);
        if (jsonString.empty())
        {
            AXLOG("Lỗi: Không thể đọc file savegame.json sau khi tạo");
            return false;
        }
    }

    rapidjson::Document doc;
    doc.Parse(jsonString.c_str());
    if (doc.HasParseError() || !doc.IsObject() || !doc.HasMember("data") || !doc["data"].IsArray())
    {
        AXLOG("Lỗi phân tích savegame.json, tạo file mới");
        if (!createSaveGame())
        {
            AXLOG("Lỗi: Không thể tạo lại savegame.json");
            return false;
        }
        jsonString = readFileContent(filename);
        doc.Parse(jsonString.c_str());
        if (doc.HasParseError() || !doc.IsObject() || !doc.HasMember("data"))
        {
            AXLOG("Lỗi: Không thể khôi phục savegame.json");
            return false;
        }
    }

    shopData.clear();
    if (!doc.HasMember("data") || !doc["data"].IsArray())
    {
        AXLOG("savegame.json không có mảng 'data', tạo file mới");
        createSaveGame();
        return loadSaveGame();
    }

    const auto& dataArray = doc["data"];
    for (rapidjson::SizeType i = 0; i < dataArray.Size(); ++i)
    {
        const auto& dataObj = dataArray[i];
        ShopData newData;

        if (dataObj.HasMember("type") && dataObj["type"].IsString())
            newData.type = dataObj["type"].GetString();
        if (dataObj.HasMember("name") && dataObj["name"].IsString())
            newData.name = dataObj["name"].GetString();
        if (dataObj.HasMember("available") && dataObj["available"].IsBool())
            newData.available = dataObj["available"].GetBool();
        if (dataObj.HasMember("level") && dataObj["level"].IsInt())
            newData.level = dataObj["level"].GetInt();
        if (dataObj.HasMember("levelValue") && dataObj["levelValue"].IsInt())
            newData.levelValue = dataObj["levelValue"].GetInt();
        if (dataObj.HasMember("cost") && dataObj["cost"].IsInt())
            newData.cost = dataObj["cost"].GetInt();
        if (dataObj.HasMember("levelLimit") && dataObj["levelLimit"].IsInt())
            newData.levelLimit = dataObj["levelLimit"].GetInt();
        if (dataObj.HasMember("valueIncrease") && dataObj["valueIncrease"].IsFloat())
            newData.valueIncrease = dataObj["valueIncrease"].GetFloat();
        if (dataObj.HasMember("valueIncrement") && dataObj["valueIncrement"].IsFloat())
            newData.valueIncrement = dataObj["valueIncrement"].GetFloat();

        shopData.push_back(newData);
    }

    shopDataVersion = 1;
    syncCharactersWithGameData();
    syncMapsWithGameData();
    GameData::getInstance()->syncStatsWithShopSystem();

    AXLOG("Đã nạp shopData từ savegame.json và đồng bộ với GameData");
    return true;
}

void ShopSystem::syncCharactersWithGameData()
{
    auto gameData  = GameData::getInstance();
    auto& entities = const_cast<std::unordered_map<std::string, std::unordered_map<std::string, EntityTemplate>>&>(
        gameData->getEntityTemplates());

    for (const auto& data : shopData)
    {
        if (data.type == "entities")
        {
            if (entities["player"].find(data.name) != entities["player"].end())
            {
                entities["player"][data.name].available = data.available;
                AXLOG("Đã đồng bộ nhân vật %s: available=%d", data.name.c_str(), data.available);
            }
        }
    }

    for (auto& data : shopData)
    {
        if (data.type == "entities" && entities["player"].find(data.name) != entities["player"].end())
        {
            data.available = entities["player"][data.name].available;
        }
    }
}

void ShopSystem::syncMapsWithGameData()
{
    auto gameData = GameData::getInstance();
    auto& maps    = const_cast<std::unordered_map<std::string, MapData>&>(gameData->getMaps());

    for (const auto& data : shopData)
    {
        if (data.type == "maps")
        {
            if (maps.find(data.name) != maps.end())
            {
                maps[data.name].available = data.available;
                AXLOG("Đã đồng bộ map %s: available=%d", data.name.c_str(), data.available);
            }
        }
    }

    for (auto& data : shopData)
    {
        if (data.type == "maps" && maps.find(data.name) != maps.end())
        {
            data.available = maps[data.name].available;
        }
    }
}

void ShopSystem::syncCoinsWithGameData(float coinMultiplier)
{
    for (auto& data : shopData)
    {
        if (data.type == "Coin")
        {
            if (data.levelValue.has_value())
            {
                int currentCoins   = data.levelValue.value();
                int increasedCoins = static_cast<int>(currentCoins * coinMultiplier);
                data.levelValue    = currentCoins + increasedCoins;
                AXLOG("Coin tăng với hệ số %.2f: %d -> %d", coinMultiplier, currentCoins, data.levelValue.value());
            }
            else
            {
                data.levelValue = 0;
            }
            shopDataVersion++;
            break;
        }
    }
    saveToFile(ax::FileUtils::getInstance()->getWritablePath() + "savegame.json");
}

int ShopSystem::getCoins() const
{
    for (const auto& data : shopData)
    {
        if (data.type == "Coin")
            return data.levelValue.value_or(0);
    }
    return 0;
}

void ShopSystem::setCoins(int value)
{
    for (auto& data : shopData)
    {
        if (data.type == "Coin")
        {
            data.levelValue = value;
            shopDataVersion++;
            saveToFile(ax::FileUtils::getInstance()->getWritablePath() + "savegame.json");
            return;
        }
    }
    shopData.push_back({"Coin", "", false, std::nullopt, value, 0, std::nullopt, std::nullopt, std::nullopt});
    shopDataVersion++;
    saveToFile(ax::FileUtils::getInstance()->getWritablePath() + "savegame.json");
}

void ShopSystem::setAvailable(const std::string& type, const std::string& name, bool available)
{
    if (type != "entities" && type != "maps")
        return;

    for (auto& data : shopData)
    {
        if (data.type == type && data.name == name)
        {
            data.available = available;
            shopDataVersion++;
            saveToFile(ax::FileUtils::getInstance()->getWritablePath() + "savegame.json");
            if (type == "entities")
                syncCharactersWithGameData();
            else if (type == "maps")
                syncMapsWithGameData();
            return;
        }
    }
    shopData.push_back(
        {type, name, available, std::nullopt, std::nullopt, 100, std::nullopt, std::nullopt, std::nullopt});
    shopDataVersion++;
    saveToFile(ax::FileUtils::getInstance()->getWritablePath() + "savegame.json");
    if (type == "entities")
        syncCharactersWithGameData();
    else if (type == "maps")
        syncMapsWithGameData();
}

int ShopSystem::getCost(const std::string& type, const std::string& name) const
{
    for (const auto& data : shopData)
    {
        if (data.type == type && data.name == name && data.cost.has_value())
            return data.cost.value();
    }
    return 0;
}

int ShopSystem::getLevel(const std::string& type, const std::string& name) const
{
    for (const auto& data : shopData)
    {
        if (data.type == type && data.name == name && data.level.has_value())
            return data.level.value();
    }
    return 0;
}

int ShopSystem::getLevelLimit(const std::string& type, const std::string& name) const
{
    for (const auto& data : shopData)
    {
        if (data.type == type && data.name == name && data.levelLimit.has_value())
            return data.levelLimit.value();
    }
    return 10;
}

float ShopSystem::getValueIncrease(const std::string& type, const std::string& name) const
{
    for (const auto& data : shopData)
    {
        if (data.type == type && data.name == name && data.valueIncrease.has_value())
            return data.valueIncrease.value();
    }
    return 0.1f;
}

float ShopSystem::getValueIncrement(const std::string& type, const std::string& name) const
{
    for (const auto& data : shopData)
    {
        if (data.type == type && data.name == name && data.valueIncrement.has_value())
            return data.valueIncrement.value();
    }
    return 0.0f;
}

int ShopSystem::getStatLevelValue(const std::string& type, const std::string& name) const
{
    for (const auto& data : shopData)
    {
        if (data.type == type && data.name == name && data.levelValue.has_value())
            return data.levelValue.value();
    }
    return 0;
}
