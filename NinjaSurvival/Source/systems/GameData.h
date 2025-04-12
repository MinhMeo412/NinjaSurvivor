#ifndef __GAMEDATA_H__
#define __GAMEDATA_H__

#include "axmol.h"
#include <string>
#include <memory> 
#include <unordered_map>
#include "components/Components.h"

struct MapData
{
    std::string name;
    std::string sprite;
    bool available;
    int mapWidth;
    int mapHeight;

    std::vector<std::string> upRightCornerTMXFile;
    std::vector<std::string> downRightCornerTMXFile;
    std::vector<std::string> upLeftCornerTMXFile;
    std::vector<std::string> downLeftCornerTMXFile;
    std::vector<std::string> upRearTMXFile;
    std::vector<std::string> downRearTMXFile;
    std::vector<std::string> leftRearTMXFile;
    std::vector<std::string> rightRearTMXFile;
    std::vector<std::string> middleTMXFile;

    MapData() : available(false) {};
};

//Cập nhật toàn bộ component entity có thể sở hữu
struct EntityTemplate
{
    std::string type;
    std::string name;
    bool available;
    std::optional<std::string> profilePhoto;
    std::optional<TransformComponent> transform;
    std::optional<SpriteComponent> sprite;
    std::optional<AnimationComponent> animation;
    std::optional<VelocityComponent> velocity;
    std::optional<SpeedComponent> speed;
    std::optional<HitboxComponent> hitbox;
    std::optional<HealthComponent> health;  
    std::optional<AttackComponent> attack;  
    std::optional<CooldownComponent> cooldown;
    std::optional<WeaponInventoryComponent> weaponInventory;
    std::optional<DurationComponent> duration;

    EntityTemplate() : available(false) {};
};

class GameData
{
private:
    static std::unique_ptr<GameData> instance;
    GameData();
    static GameData* createInstance();

    //Danh sách lưu dữ liệu map và entities load từ json
    std::unordered_map<std::string, MapData> maps;
    std::unordered_map<std::string, std::unordered_map<std::string, EntityTemplate>> entityTemplates;

    //Lưu dữ liệu map và character được chọn trong scene
    std::string selectedCharacterName;
    std::string selectedMapName;

    // Parse JSON và lưu vào unordered_map 
    bool loadMapData(const std::string& jsonString);
    bool loadEntityData(const std::string& jsonString);
    std::string readFileContent(const std::string& filename);

public:
    ~GameData();
    static GameData* getInstance();

    bool loadMapDataFromFile(const std::string& filename);
    bool loadEntityDataFromFile(const std::string& filename);

    // Lấy danh sách toàn bộ map
    const std::unordered_map<std::string, MapData>& getMaps() const;
    //Lấy danh sách toàn bộ entities (có thể dùng .at(type).at(name) để truy cập cụ thể)
    const std::unordered_map<std::string, std::unordered_map<std::string, EntityTemplate>>& getEntityTemplates() const;

    // Sửa dữ liệu (chưa dùng)
    void setMapAvailable(const std::string& name, bool available);

    void setCharacterAvailable(const std::string& name, bool available);

    void setSelectedCharacter(const std::string& characterName);
    std::string getSelectedCharacter() const;
    void setSelectedMap(const std::string& mapName);
    std::string getSelectedMap() const;

    
    std::string findTypeByName(
        const std::unordered_map<std::string, std::unordered_map<std::string, EntityTemplate>>& entityTemplates,
        const std::string& name);
};

#endif  // __GAMEDATA_H__
