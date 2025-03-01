#include "GameData.h"
#include "rapidjson/document.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::unique_ptr<GameData> GameData::instance = nullptr;

GameData::GameData() {}

GameData::~GameData() {}

GameData* GameData::createInstance()
{
    return new GameData();
}

GameData* GameData::getInstance()
{
    if (!instance)
    {
        AXLOG("Creating GameData instance");
        instance.reset(createInstance());
    }
    else
    {
        AXLOG("GameData instance already exists");
    }
    return instance.get();
}

bool GameData::loadMapData(const std::string& jsonString)
{
    rapidjson::Document doc;
    doc.Parse(jsonString.c_str());

    if (doc.HasParseError() || !doc.IsObject() || !doc.HasMember("maps") || !doc["maps"].IsArray())
    {
        AXLOG("Error parsing JSON or invalid format");
        return false;
    }

    maps.clear();
    const auto& mapsArray = doc["maps"];
    for (rapidjson::SizeType i = 0; i < mapsArray.Size(); i++)
    {
        const auto& mapObj = mapsArray[i];
        MapData map;
        map.tmxFile   = mapObj["tmxFile"].GetString();
        map.name      = mapObj["name"].GetString();
        map.sprite    = mapObj["sprite"].GetString();
        map.available = mapObj["available"].GetBool();

        maps[map.name] = map;  // Lưu vào map với key là name
    }
    return true;
}

std::string GameData::readFileContent(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        std::cerr << "Error opening file: " << filename << std::endl;
        return "";
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool GameData::loadMapDataFromFile(const std::string& filename)
{
    std::string jsonString = readFileContent(filename);
    if (jsonString.empty())
        return false;

    return loadMapData(jsonString);
}

const std::unordered_map<std::string, MapData>& GameData::getMaps() const
{
    return maps;
}

const MapData* GameData::getMap(const std::string& name) const
{
    auto it = maps.find(name);
    if (it != maps.end())
    {
        return &(it->second);
    }
    return nullptr;
}

void GameData::setMapAvailable(const std::string& name, bool available)
{
    auto it = maps.find(name);
    if (it != maps.end())
    {
        it->second.available = available;
    }
}
