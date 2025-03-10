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
        AXLOG("Error parsing JSON or invalid format for maps");
        return false;
    }

    maps.clear();
    const auto& mapsArray = doc["maps"];
    for (rapidjson::SizeType i = 0; i < mapsArray.Size(); i++)
    {
        const auto& mapObj = mapsArray[i];
        MapData map;
        map.tmxFile = mapObj["tmxFile"].GetString();
        map.name = mapObj["name"].GetString();
        map.sprite = mapObj["sprite"].GetString();
        map.available = mapObj["available"].GetBool();

        maps[map.name] = map;  // Lưu vào map với key là name
    }
    return true;
}

bool GameData::loadEntityData(const std::string& jsonString)
{
    rapidjson::Document doc;
    doc.Parse(jsonString.c_str());

    //Kiểm tra dữ liệu json
    if (doc.HasParseError() || !doc.IsObject() || !doc.HasMember("entities") || !doc["entities"].IsArray())
    {
        AXLOG("Error parsing JSON or invalid format for entities");
        return false;
    }

    entityTemplates.clear();

    //Lấy mảng entities
    const auto& entitiesArray = doc["entities"];

    //Duyệt qua từng phần tử trong mảng entities
    for (rapidjson::SizeType i = 0; i < entitiesArray.Size(); i++)
    {
        //Lấy data tại vị trí phần tử i trong mảng entities
        const auto& entityObj = entitiesArray[i];
        EntityTemplate templ;

        //Kiểu data luôn có trong mọi entity
        //Type
        if (entityObj.HasMember("type") && entityObj["type"].IsString())
        {
            templ.type = entityObj["type"].GetString();
        }
        else
        {
            AXLOG("Error: Entity type must be a string");
            return false;
        }
        //Name
        if (entityObj.HasMember("name") && entityObj["name"].IsString())
        {
            templ.name = entityObj["name"].GetString();
        }
        else
        {
            AXLOG("Error: Entity name must be a string");
            return false;
        }
        //Available
        if (entityObj.HasMember("available") && entityObj["available"].IsBool())
        {
            templ.available = entityObj["available"].GetBool();
        }
        else
        {
            AXLOG("Error: Entity available must be a bool");
            return false;
        }

        //Kiểu data có thể có hoặc không
        //Kiểm tra có phần tử components trong entity không
        if (entityObj.HasMember("components") && entityObj["components"].IsObject())
        {
            //Lấy data components
            const auto& components = entityObj["components"];
            
            //Kiểm tra:
            //Sprite
            if (components.HasMember("sprite") && components["sprite"].IsObject())
            {
                const auto& sprite    = components["sprite"];
                std::string frameFile = sprite.HasMember("frame") ? sprite["frame"].GetString() : "";
                std::string gameSceneFile = sprite.HasMember("gameSceneFrame") ? sprite["gameSceneFrame"].GetString() : "";
                templ.sprite = SpriteComponent(frameFile, gameSceneFile);
            }

            //Animation
            if (components.HasMember("animation") && components["animation"].IsObject())
            {
                //Lấy dữ liệu animation
                const auto& anim = components["animation"];

                //Khai báo một map để lưu cặp "state","frame animation list"
                std::unordered_map<std::string, std::vector<std::string>> frameMap;

                //Kiểm tra trong animation có frames không
                if (anim.HasMember("frames") && anim["frames"].IsObject())
                {
                    //Lấy dữ liệu frames
                    const auto& frameObj = anim["frames"];
                    //Duyệt qua mọi cặp key(kiểu tên animation) - value(danh sách frame)
                    for (auto it = frameObj.MemberBegin(); it != frameObj.MemberEnd(); it++)
                    {
                        //Lấy state = tên entity + tên animation
                        std::string state = templ.name + it->name.GetString();
                        //Lấy mảng các frame
                        const auto& frameArray = it->value;
                        //Tạo vector để lưu các frame từ mảng frameArray
                        std::vector<std::string> frames;
                        if (frameArray.IsArray())
                        {
                            for (rapidjson::SizeType j = 0; j < frameArray.Size(); j++)
                            {
                                frames.push_back(frameArray[j].GetString());
                            }
                        }
                        //Lưu vào map cặp key(state - tên của animation) - value(danh sách các frame cho animation)
                        frameMap[state] = frames;
                    }
                }
                //Kiểm tra các object còn lại và thêm vào nếu có
                templ.animation =
                    AnimationComponent(anim.HasMember("plist") ? anim["plist"].GetString() : "", frameMap,
                                       anim.HasMember("frameDuration") ? anim["frameDuration"].GetFloat() : 0.2f);
            }
            
            // Transform
            if (components.HasMember("transform") && components["transform"].IsObject())
            {
                const auto& trans = components["transform"];
                if (trans.HasMember("x") && trans["x"].IsFloat() && trans.HasMember("y") && trans["y"].IsFloat())
                {
                    float scale     = trans.HasMember("scale") ? trans["scale"].GetFloat() : 1.0f;
                    templ.transform = TransformComponent{trans["x"].GetFloat(), trans["y"].GetFloat(),scale};
                }
                else
                {
                    AXLOG("Warning: transform component missing x or y for entity %s-%s", templ.type.c_str(),
                          templ.name.c_str());
                }
            }

            // Velocity
            if (components.HasMember("velocity") && components["velocity"].IsObject())
            {
                const auto& vel = components["velocity"];
                if (vel.HasMember("vx") && vel["vx"].IsFloat() && vel.HasMember("vy") && vel["vy"].IsFloat())
                {
                    templ.velocity = VelocityComponent{vel["vx"].GetFloat(), vel["vy"].GetFloat()};
                }
                else
                {
                    AXLOG("Warning: velocity component missing vx or vy for entity %s-%s", templ.type.c_str(),
                          templ.name.c_str());
                }
            }
        }
        entityTemplates[templ.type][templ.name] = templ;
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

//Nhận tên file .json
bool GameData::loadMapDataFromFile(const std::string& filename)
{
    std::string jsonString = readFileContent(filename);
    if (jsonString.empty())
        return false;

    return loadMapData(jsonString);
}

// Nhận tên file .json
bool GameData::loadEntityDataFromFile(const std::string& filename)
{
    std::string jsonString = readFileContent(filename);
    if (jsonString.empty())
        return false;
    return loadEntityData(jsonString);
}

//lấy danh sách map
const std::unordered_map<std::string, MapData>& GameData::getMaps() const
{
    return maps;
}

const std::unordered_map<std::string, std::unordered_map<std::string, EntityTemplate>>& GameData::getEntityTemplates() const
{
    return entityTemplates;
}

//tìm map theo tên map
const MapData* GameData::getMap(const std::string& name) const
{
    auto it = maps.find(name);
    if (it != maps.end())
    {
        return &(it->second);
    }
    return nullptr;
}


//Sửa map thành available (cần thay đổi)
void GameData::setMapAvailable(const std::string& name, bool available)
{
    auto it = maps.find(name);
    if (it != maps.end())
    {
        it->second.available = available;
    }
}

void GameData::setSelectedCharacter(const std::string& characterName)
{
    selectedCharacterName = characterName;
}

std::string GameData::getSelectedCharacter() const
{
    return selectedCharacterName;
}

void GameData::setSelectedMap(const std::string& mapName)
{
    selectedMapName = mapName;
}

std::string GameData::getSelectedMap() const
{
    return selectedMapName;
}
