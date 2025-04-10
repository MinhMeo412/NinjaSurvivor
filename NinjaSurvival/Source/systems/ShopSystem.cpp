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
    if (!instance)
    {
        AXLOG("Creating ShopSystem instance");
        instance.reset(createInstance());
        // Tạo savegame.json nếu chưa tồn tại
        instance->createSaveGame();
    }
    else
    {
        AXLOG("ShopSystem instance already exists");
    }
    return instance.get();
}

std::string ShopSystem::readFileContent(const std::string& filename)
{
    std::string fullPath = ax::FileUtils::getInstance()->fullPathForFilename(filename);
    if (fullPath.empty())
    {
        AXLOG("Error: File %s not found", filename.c_str());
        return "";
    }
    std::string content = ax::FileUtils::getInstance()->getStringFromFile(fullPath);
    if (content.empty())
    {
        AXLOG("Error: Failed to read content from %s", filename.c_str());
    }
    else
    {
        AXLOG("Successfully read %s: %d bytes", filename.c_str(), content.size());
    }
    return content;
}

bool ShopSystem::createSaveGame()
{
    std::string filePath = ax::FileUtils::getInstance()->getWritablePath() + "savegame.json";

    if (isSaveGameExist())
    {
        AXLOG("Save game already exists at: %s", filePath.c_str());
        return true;
    }

    // Tạo dữ liệu mặc định với các biến mới
    shopData = {
        {"Coin", "", false, std::nullopt, 1000, 0, std::nullopt, std::nullopt},  // Coin mặc định
        {"Stat", "Health", false, 0, 100, 50, 10, 0.1f},  // Stat mặc định: cost=50, levelLimit=10, valueIncrease=0.1
        {"Stat", "Attack", false, 0, 10, 50, 10, 0.1f},   // Stat mặc định: cost=50, levelLimit=10, valueIncrease=0.1
        {"entities", "Ninja", true, std::nullopt, std::nullopt, 100, std::nullopt,
         std::nullopt},  // Character: cost=100
        {"entities", "Master", false, std::nullopt, std::nullopt, 200, std::nullopt,
         std::nullopt},                                                                    // Character: cost=200
        {"maps", "Map", true, std::nullopt, std::nullopt, 0, std::nullopt, std::nullopt},  // Map: cost=0
        {"maps", "Large Map", false, std::nullopt, std::nullopt, 150, std::nullopt, std::nullopt}  // Map: cost=150
    };

    return saveToFile(filePath);
}

bool ShopSystem::isSaveGameExist() const
{
    std::string filePath = ax::FileUtils::getInstance()->getWritablePath() + "savegame.json";
    return ax::FileUtils::getInstance()->isFileExist(filePath);
}

void ShopSystem::syncCharactersWithGameData()
{
    auto gameData  = GameData::getInstance();
    auto& entities = const_cast<std::unordered_map<std::string, std::unordered_map<std::string, EntityTemplate>>&>(
        gameData->getEntityTemplates());

    // Đồng bộ từ shopData sang GameData
    for (const auto& data : shopData)
    {
        if (data.type == "entities")
        {
            if (entities["player"].find(data.name) != entities["player"].end())
            {
                entities["player"][data.name].available = data.available;
                AXLOG("Synced character %s: available=%d", data.name.c_str(), data.available);
            }
        }
    }

    // Đồng bộ ngược từ GameData sang shopData
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

    // Đồng bộ từ shopData sang GameData
    for (const auto& data : shopData)
    {
        if (data.type == "maps")
        {
            if (maps.find(data.name) != maps.end())
            {
                maps[data.name].available = data.available;
                AXLOG("Synced map %s: available=%d", data.name.c_str(), data.available);
            }
        }
    }

    // Đồng bộ ngược từ GameData sang shopData
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
                AXLOG("Coins increased by multiplier %.2f: %d -> %d", coinMultiplier, currentCoins,
                      data.levelValue.value());
            }
            else
            {
                data.levelValue = 0;
            }
            break;
        }
    }
    saveToFile(ax::FileUtils::getInstance()->getWritablePath() + "savegame.json");
}

bool ShopSystem::loadSaveGame()
{
    std::string filePath = ax::FileUtils::getInstance()->getWritablePath() + "savegame.json";

    if (!isSaveGameExist())
    {
        AXLOG("No save game found at: %s", filePath.c_str());
        if (!createSaveGame())
        {
            AXLOG("Failed to create save game during load");
            return false;
        }
        syncCharactersWithGameData();
        syncMapsWithGameData();
        return true;
    }

    std::string jsonString = ax::FileUtils::getInstance()->getStringFromFile(filePath);
    if (jsonString.empty())
    {
        AXLOG("Failed to read save game content from: %s", filePath.c_str());
        return false;
    }

    Document doc;
    doc.Parse(jsonString.c_str());

    if (doc.HasParseError() || !doc.IsObject() || !doc.HasMember("data") || !doc["data"].IsArray())
    {
        AXLOG("Error parsing save game JSON or invalid format");
        return false;
    }

    const auto& dataArray = doc["data"];

    // Cập nhật shopData dựa trên dữ liệu từ savegame.json
    for (SizeType i = 0; i < dataArray.Size(); i++)
    {
        const auto& dataObj = dataArray[i];
        std::string type, name;
        bool available = false;
        int level = 0, levelValue = 0, cost = 0, levelLimit = 0;
        float valueIncrease = 0.0f;

        if (dataObj.HasMember("type") && dataObj["type"].IsString())
            type = dataObj["type"].GetString();
        else
            continue;

        if (dataObj.HasMember("name") && dataObj["name"].IsString())
            name = dataObj["name"].GetString();

        if (dataObj.HasMember("available") && dataObj["available"].IsBool())
            available = dataObj["available"].GetBool();

        if (dataObj.HasMember("level") && dataObj["level"].IsInt())
            level = dataObj["level"].GetInt();

        if (dataObj.HasMember("levelValue") && dataObj["levelValue"].IsInt())
            levelValue = dataObj["levelValue"].GetInt();

        if (dataObj.HasMember("cost") && dataObj["cost"].IsInt())
            cost = dataObj["cost"].GetInt();

        if (dataObj.HasMember("levelLimit") && dataObj["levelLimit"].IsInt())
            levelLimit = dataObj["levelLimit"].GetInt();

        if (dataObj.HasMember("valueIncrease") && dataObj["valueIncrease"].IsFloat())
            valueIncrease = dataObj["valueIncrease"].GetFloat();

        // Tìm và cập nhật mục tương ứng trong shopData
        bool found = false;
        for (auto& data : shopData)
        {
            if (data.type == type && data.name == name)
            {
                if (type == "Coin")
                {
                    data.levelValue = levelValue;
                }
                else if (type == "Stat")
                {
                    data.level         = level;
                    data.levelValue    = levelValue;
                    data.valueIncrease = valueIncrease;
                }
                else if (type == "entities" || type == "maps")
                {
                    data.available = available;
                }
                found = true;
                break;
            }
        }
        // Nếu không tìm thấy mục trong shopData, thêm mới
        if (!found)
        {
            ShopData newData;
            newData.type = type;
            newData.name = name;
            if (type == "Coin")
            {
                newData.levelValue = levelValue;
            }
            else if (type == "Stat")
            {
                newData.level         = level;
                newData.levelValue    = levelValue;
                newData.valueIncrease = valueIncrease;
                newData.cost          = cost;
                newData.levelLimit    = levelLimit;
            }
            else if (type == "entities" || type == "maps")
            {
                newData.available = available;
                newData.cost      = cost;
            }
            shopData.push_back(newData);
        }
    }

    syncCharactersWithGameData();
    syncMapsWithGameData();
    AXLOG("Loaded save game: data count=%d", shopData.size());
    return true;
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
        dataArray.PushBack(dataObj, allocator);
    }

    doc.AddMember("data", dataArray, allocator);

    StringBuffer buffer;
    Writer<StringBuffer> writer(buffer);
    doc.Accept(writer);
    std::string jsonData = buffer.GetString();

    if (ax::FileUtils::getInstance()->writeStringToFile(jsonData, filename))
    {
        AXLOG("Saved to %s: %s", filename.c_str(), jsonData.c_str());
        return true;
    }
    else
    {
        AXLOG("Failed to save to %s", filename.c_str());
        return false;
    }
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
            saveToFile(ax::FileUtils::getInstance()->getWritablePath() + "savegame.json");
            return;
        }
    }
    shopData.push_back({"Coin", "", false, std::nullopt, value, 0, std::nullopt, std::nullopt});
    saveToFile(ax::FileUtils::getInstance()->getWritablePath() + "savegame.json");
}

void ShopSystem::setStatLevel(const std::string& name, int level, int levelValue, float valueIncrease)
{
    for (auto& data : shopData)
    {
        if (data.type == "Stat" && data.name == name)
        {
            if (data.levelLimit.has_value() && level > data.levelLimit.value())
            {
                AXLOG("Cannot set stat level for %s: level %d exceeds limit %d", name.c_str(), level,
                      data.levelLimit.value());
                return;
            }

            data.level         = level;
            data.levelValue    = levelValue;
            data.valueIncrease = valueIncrease;
            saveToFile(ax::FileUtils::getInstance()->getWritablePath() + "savegame.json");
            return;
        }
    }
    shopData.push_back({"Stat", name, false, level, levelValue, 50, 10, valueIncrease});
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
            saveToFile(ax::FileUtils::getInstance()->getWritablePath() + "savegame.json");
            if (type == "entities")
                syncCharactersWithGameData();
            else if (type == "maps")
                syncMapsWithGameData();
            return;
        }
    }
    shopData.push_back({type, name, available, std::nullopt, std::nullopt, 100, std::nullopt, std::nullopt});
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
    return 0.0f;
}
