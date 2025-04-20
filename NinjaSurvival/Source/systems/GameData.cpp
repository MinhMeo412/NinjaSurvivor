#include "GameData.h"
#include "ShopSystem.h"
#include "rapidjson/document.h"


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
        AXLOG("Khởi tạo GameData instance");
        instance.reset(createInstance());
        instance->lastShopSyncVersion = 0;  // Đặt lại để buộc đồng bộ lúc khởi động

        // Tải dữ liệu mặc định từ file JSON
        if (!instance->loadMapDataFromFile("maps.json"))
        {
            AXLOG("Lỗi: Không thể tải maps.json");
        }
        if (!instance->loadEntityDataFromFile("entities.json"))
        {
            AXLOG("Lỗi: Không thể tải entities.json");
        }

        // Tải và đồng bộ với savegame.json
        auto shopData = ShopSystem::getInstance();
        if (shopData->loadSaveGame())
        {
            AXLOG("Đã tải thành công savegame.json trong GameData::getInstance()");
            // Đồng bộ dữ liệu từ ShopSystem vào GameData
            shopData->syncCharactersWithGameData();
            shopData->syncMapsWithGameData();
        }
        else
        {
            AXLOG("Không tìm thấy savegame.json hoặc tải thất bại, tạo dữ liệu mặc định");
            shopData->createSaveGame();           // Tạo file mới nếu tải thất bại
            instance->syncStatsWithShopSystem();  // Đồng bộ với dữ liệu mặc định
        }
    }
    else
    {
        //AXLOG("GameData instance đã tồn tại");
    }
    return instance.get();
}
bool GameData::loadEntityData(const std::string& jsonString)
{
    rapidjson::Document doc;
    doc.Parse(jsonString.c_str());

    // Kiểm tra dữ liệu json
    if (doc.HasParseError() || !doc.IsObject() || !doc.HasMember("entities") || !doc["entities"].IsArray())
    {
        AXLOG("Error parsing JSON or invalid format for entities");
        return false;
    }

    entityTemplates.clear();

    // Lấy mảng entities
    const auto& entitiesArray = doc["entities"];

    // Duyệt qua từng phần tử trong mảng entities
    for (rapidjson::SizeType i = 0; i < entitiesArray.Size(); i++)
    {
        // Lấy data tại vị trí phần tử i trong mảng entities
        const auto& entityObj = entitiesArray[i];
        EntityTemplate templ;

        // Kiểu data luôn có trong mọi entity
        // Type
        if (entityObj.HasMember("type") && entityObj["type"].IsString())
        {
            templ.type = entityObj["type"].GetString();
        }
        else
        {
            AXLOG("Error: Entity type must be a string");
            return false;
        }
        // Name
        if (entityObj.HasMember("name") && entityObj["name"].IsString())
        {
            templ.name = entityObj["name"].GetString();
        }
        else
        {
            AXLOG("Error: Entity name must be a string");
            return false;
        }
        // Available
        if (entityObj.HasMember("available") && entityObj["available"].IsBool())
        {
            templ.available = entityObj["available"].GetBool();
        }
        else
        {
            AXLOG("Error: Entity available must be a bool");
            return false;
        }

        // Profile Image
        if (entityObj.HasMember("profileImage") && entityObj["profileImage"].IsString())
        {
            templ.profilePhoto = entityObj["profileImage"].GetString();
        }

        // Kiểu data có thể có hoặc không
        // Kiểm tra có phần tử components trong entity không
        if (entityObj.HasMember("components") && entityObj["components"].IsObject())
        {
            // Lấy data components
            const auto& components = entityObj["components"];

            // Kiểm tra:
            // Sprite
            if (components.HasMember("sprite") && components["sprite"].IsObject())
            {
                const auto& sprite = components["sprite"];
                std::string plist  = sprite.HasMember("plist") ? sprite["plist"].GetString() : "";
                std::string gameSceneFrame =
                    sprite.HasMember("gameSceneFrame") ? sprite["gameSceneFrame"].GetString() : "";
                templ.sprite = SpriteComponent(gameSceneFrame, plist);
            }

            // Animation
            if (components.HasMember("animation") && components["animation"].IsObject())
            {
                // Lấy dữ liệu animation
                const auto& anim = components["animation"];

                // Khai báo một map để lưu cặp "state","frame animation list"
                std::unordered_map<std::string, std::vector<std::string>> frameMap;

                // Kiểm tra trong animation có frames không
                if (anim.HasMember("frames") && anim["frames"].IsObject())
                {
                    // Lấy dữ liệu frames
                    const auto& frameObj = anim["frames"];
                    // Duyệt qua mọi cặp key(kiểu tên animation) - value(danh sách frame)
                    for (auto it = frameObj.MemberBegin(); it != frameObj.MemberEnd(); it++)
                    {
                        // Lấy state = tên entity + tên animation
                        std::string state = templ.name + it->name.GetString();
                        AXLOG("Kiểm tra state được lưu");
                        AXLOG("%s", state.c_str());
                        // Lấy mảng các frame
                        const auto& frameArray = it->value;
                        // Tạo vector để lưu các frame từ mảng frameArray
                        std::vector<std::string> frames;
                        if (frameArray.IsArray())
                        {
                            for (rapidjson::SizeType j = 0; j < frameArray.Size(); j++)
                            {
                                frames.push_back(frameArray[j].GetString());
                            }
                        }
                        // Lưu vào map cặp key(state - tên của animation) - value(danh sách các frame cho animation)
                        frameMap[state] = frames;
                    }
                }
                // Kiểm tra các object còn lại và thêm vào nếu có
                templ.animation = AnimationComponent(
                    frameMap, anim.HasMember("frameDuration") ? anim["frameDuration"].GetFloat() : 0.2f);
            }

            // Transform
            if (components.HasMember("transform") && components["transform"].IsObject())
            {
                const auto& trans = components["transform"];
                if (trans.HasMember("x") && trans["x"].IsFloat() && trans.HasMember("y") && trans["y"].IsFloat())
                {
                    float scale     = trans.HasMember("scale") ? trans["scale"].GetFloat() : 1.0f;
                    templ.transform = TransformComponent{trans["x"].GetFloat(), trans["y"].GetFloat(), scale};
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

            // Hitbox
            if (components.HasMember("hitbox") && components["hitbox"].IsObject())
            {
                const auto& hitb = components["hitbox"];
                if (hitb.HasMember("width") && hitb["width"].IsFloat() && hitb.HasMember("height") &&
                    hitb["height"].IsFloat())
                {
                    templ.hitbox = HitboxComponent{hitb["width"].GetFloat(), hitb["height"].GetFloat()};
                }
                else
                {
                    AXLOG("Warning: hitbox component missing width or height for entity %s-%s", templ.type.c_str(),
                          templ.name.c_str());
                }
            }

            // Health
            if (components.HasMember("health") && components["health"].IsObject())
            {
                const auto& health = components["health"];
                if (health.HasMember("maxHealth") && health["maxHealth"].IsFloat())
                {
                    templ.health = HealthComponent{health["maxHealth"].GetFloat()};
                }
            }

            // Attack
            if (components.HasMember("attack") && components["attack"].IsObject())
            {
                const auto& attack = components["attack"];
                if (attack.HasMember("baseDamage") && attack["baseDamage"].IsFloat())
                {
                    float baseDamage = attack["baseDamage"].GetFloat();
                    float damageMultiplier =
                        attack.HasMember("damageMultiplier") && attack["damageMultiplier"].IsFloat()
                            ? attack["damageMultiplier"].GetFloat()
                            : 0.0f;
                    templ.attack = AttackComponent{baseDamage, damageMultiplier};
                }
            }

            // Cooldown
            if (components.HasMember("cooldown") && components["cooldown"].IsFloat())
            {
                templ.cooldown = CooldownComponent{components["cooldown"].GetFloat()};
            }

            // Speed
            if (components.HasMember("speed") && components["speed"].IsFloat())
            {
                templ.speed = SpeedComponent{components["speed"].GetFloat()};
            }

            // WeaponInventory (default weapon)
            if (components.HasMember("weapon") && components["weapon"].IsString())
            {
                templ.weaponInventory = WeaponInventoryComponent{components["weapon"].GetString()};
            }

            // Duration
            if (components.HasMember("duration") && components["duration"].IsFloat())
            {
                templ.duration = DurationComponent{components["duration"].GetFloat()};
            }
        }
        entityTemplates[templ.type][templ.name] = templ;
    }
    return true;
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
        map.name = mapObj["name"].GetString();
        map.sprite = mapObj["sprite"].GetString();
        map.available = mapObj["available"].GetBool();
        const auto& mapS = mapObj["mapSize"];
        map.mapWidth     = mapS["width"].GetInt();
        map.mapHeight     = mapS["height"].GetInt();

        // Phương thức load TMX file arrays chung
        auto loadTMXArray = [&](const char* key, std::vector<std::string>& target) {
            if (mapObj.HasMember(key) && mapObj[key].IsArray())
            {
                const auto& array = mapObj[key];
                for (rapidjson::SizeType j = 0; j < array.Size(); j++)
                    target.push_back(array[j].GetString());
            }
        };

        loadTMXArray("upRightCornerTMXFile", map.upRightCornerTMXFile);
        loadTMXArray("downRightCornerTMXFile", map.downRightCornerTMXFile);
        loadTMXArray("upLeftCornerTMXFile", map.upLeftCornerTMXFile);
        loadTMXArray("downLeftCornerTMXFile", map.downLeftCornerTMXFile);
        loadTMXArray("upRearTMXFile", map.upRearTMXFile);
        loadTMXArray("downRearTMXFile", map.downRearTMXFile);
        loadTMXArray("leftRearTMXFile", map.leftRearTMXFile);
        loadTMXArray("rightRearTMXFile", map.rightRearTMXFile);
        loadTMXArray("middleTMXFile", map.middleTMXFile);

        maps[map.name] = map;  // Lưu vào map với key là name
    }
    return true;
}

void GameData::syncStatsWithShopSystem()
{
    auto shop = ShopSystem::getInstance();

    // Đọc chỉ số cơ bản từ entities.json
    std::string filename   = ax::FileUtils::getInstance()->fullPathForFilename("entities.json");
    std::string jsonString = ax::FileUtils::getInstance()->getStringFromFile(filename);
    if (jsonString.empty())
    {
        AXLOG("Lỗi: Không thể đọc entities.json");
        return;
    }

    rapidjson::Document doc;
    doc.Parse(jsonString.c_str());
    if (doc.HasParseError() || !doc.IsObject() || !doc.HasMember("entities") || !doc["entities"].IsArray())
    {
        AXLOG("Lỗi: Phân tích entities.json thất bại");
        return;
    }

    // Lưu chỉ số cơ bản
    std::unordered_map<std::string, std::unordered_map<std::string, EntityTemplate>> baseTemplates;
    const auto& entitiesArray = doc["entities"];
    for (rapidjson::SizeType i = 0; i < entitiesArray.Size(); ++i)
    {
        const auto& entity = entitiesArray[i];
        if (!entity.HasMember("type") || !entity["type"].IsString() || !entity.HasMember("name") ||
            !entity["name"].IsString())
            continue;

        std::string type = entity["type"].GetString();
        std::string name = entity["name"].GetString();
        EntityTemplate templ;

        if (entity.HasMember("components"))
        {
            const auto& components = entity["components"];
            if (components.HasMember("health") && components["health"].IsObject() &&
                components["health"].HasMember("maxHealth"))
                templ.health = HealthComponent{static_cast<float>(components["health"]["maxHealth"].GetDouble())};
            if (components.HasMember("attack") && components["attack"].IsObject() &&
                components["attack"].HasMember("damageMultiplier"))
                templ.attack =
                    AttackComponent{static_cast<float>(components["attack"]["damageMultiplier"].GetDouble()), 1.0f};
            if (components.HasMember("speed") && components["speed"].IsFloat())
                templ.speed = SpeedComponent{static_cast<float>(components["speed"].GetFloat())};
            if (components.HasMember("cooldown") && components["cooldown"].IsFloat())
                templ.cooldown = CooldownComponent{static_cast<float>(components["cooldown"].GetFloat())};
        }

        baseTemplates[type][name] = templ;
    }

    // Cập nhật entityTemplates
    for (auto& [type, templates] : entityTemplates)
    {
        if (type != "player" && type != "weapon_melee" && type != "weapon_projectile")
        {
            AXLOG("Bỏ qua type không được hỗ trợ: %s", type.c_str());
            continue;
        }

        for (auto& [name, templ] : templates)
        {
            if (baseTemplates[type].find(name) == baseTemplates[type].end())
            {
                AXLOG("Lỗi: Không tìm thấy %s trong entities.json cho type %s", name.c_str(), type.c_str());
                continue;
            }
            auto& baseTempl = baseTemplates[type][name];

            // Cập nhật Health 
            if (type == "player" && templ.health.has_value() && baseTempl.health.has_value())
            {
                float baseHealth = baseTempl.health->maxHealth;
                float healthBuff = shop->getStatLevelValue("Stat", "Health");
                templ.health     = HealthComponent{baseHealth + baseHealth * healthBuff};
                AXLOG("Đồng bộ Health cho %s (%s): cơ bản=%.2f, tăng=%.2f, cuối=%.2f", name.c_str(), type.c_str(),
                      baseHealth, healthBuff, templ.health->maxHealth);
            }


            // Cập nhật Speed 
            if (type == "player" && templ.speed.has_value() && baseTempl.speed.has_value())
            {
                float baseSpeed = baseTempl.speed->speed;
                float speedBuff = shop->getStatLevelValue("Stat", "Speed");
                templ.speed     = SpeedComponent{baseSpeed + baseSpeed * speedBuff};
                AXLOG("Đồng bộ Speed cho %s (%s): cơ bản=%.2f, tăng=%.2f, cuối=%.2f", name.c_str(), type.c_str(),
                      baseSpeed, speedBuff, templ.speed->speed);
            }

            // kéo sâu xuống entities.json check
            // Cập nhật Cooldown 
            if (templ.cooldown.has_value() && baseTempl.cooldown.has_value())
            {
                float baseCooldown = baseTempl.cooldown->cooldownDuration;
                float cooldownBuff = shop->getStatLevelValue("Stat", "ReduceCooldown");

                if (type == "weapon_melee" || type == "weapon_projectile")
                {
                    templ.cooldown = CooldownComponent{baseCooldown * (1.0f - cooldownBuff)};
                    AXLOG("Đồng bộ Cooldown cho %s (%s): cơ bản=%.2f, giảm=%.2f, cuối=%.2f", name.c_str(), type.c_str(),
                          baseCooldown, cooldownBuff, templ.cooldown->cooldownDuration);
                }
                else
                {
                    AXLOG("Lỗi: Type %s không được hỗ trợ cho Cooldown", type.c_str());
                }
                // Cập nhật Attack
                if (type == "weapon_melee" || type == "weapon_projectile")
                {
                    float baseMultiplier           = baseTempl.attack->damageMultiplier;
                    float attackBuff               = shop->getStatLevelValue("Stat", "Attack");
                    templ.attack->damageMultiplier = baseMultiplier + baseMultiplier * attackBuff;
                    AXLOG("Đồng bộ Attack cho %s (%s): baseMultiplier=%.2f, attackBuff=%.2f, damageMultiplier=%.2f",
                          name.c_str(), type.c_str(), baseMultiplier, attackBuff, templ.attack->damageMultiplier);
                }
            }
        }   
    }

    lastShopSyncVersion = shop->getShopDataVersion();
    AXLOG("Hoàn tất đồng bộ shop, phiên bản=%d", lastShopSyncVersion);
}

std::string GameData::readFileContent(const std::string& filename)
{
    // Dễ gây lỗi khi chạy trên android (chỉ phù hợp PC)
    //std::ifstream file(filename);
    //if (!file)
    //{
    //    std::cerr << "Error opening file: " << filename << std::endl;
    //    return "";
    //}
    //std::stringstream buffer;
    //buffer << file.rdbuf();
    //return buffer.str();

    //Sử dụng FileUtils của Axmol
    // Lấy đường dẫn đầy đủ từ FileUtils
    std::string fullPath = ax::FileUtils::getInstance()->fullPathForFilename(filename);
    if (fullPath.empty()) {
        AXLOG("Error: File %s not found in assets", filename.c_str());
        return "";
    }
    // Đọc nội dung tệp
    std::string content = ax::FileUtils::getInstance()->getStringFromFile(fullPath);
    if (content.empty()) {
        AXLOG("Error: Failed to read content from %s (file might be empty or inaccessible)", filename.c_str());
    } else {
        AXLOG("Successfully read %s: %d bytes", filename.c_str(), content.size());
    }
    return content;
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

//Sửa map thành available (cần thay đổi)
void GameData::setMapAvailable(const std::string& name, bool available)
{
    auto it = maps.find(name);
    if (it != maps.end())
    {
        it->second.available = available;
    }
}

void GameData::setCharacterAvailable(const std::string& name, bool available)
{
    if (entityTemplates.find("player") != entityTemplates.end())
    {
        auto& playerTemplates = entityTemplates["player"];
        if (playerTemplates.find(name) != playerTemplates.end())
        {
            playerTemplates[name].available = available;
            AXLOG("Set character %s available=%d in GameData", name.c_str(), available);
        }
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



std::string GameData::findTypeByName(
    const std::unordered_map<std::string, std::unordered_map<std::string, EntityTemplate>>& entityTemplates,
    const std::string& name)
{
    for (const auto& [typeKey, nameMap] : entityTemplates)
    {
        if (nameMap.find(name) != nameMap.end())
        {
            return typeKey;  // Tìm thấy type chứa name
        }
    }
    // Không tìm thấy -> trả về rỗng
    return "";
}
