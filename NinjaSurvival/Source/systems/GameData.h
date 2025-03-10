#ifndef __GAMEDATA_H__
#define __GAMEDATA_H__

#include "axmol.h"
#include <string>
#include <memory> 
#include <unordered_map>
#include "components/Components.h"

struct MapData
{
    std::string tmxFile;
    std::string name;
    std::string sprite;
    bool available;

    MapData() : available(false) {};
};

//Cập nhật toàn bộ component entity có thể sở hữu
struct EntityTemplate
{
    std::string type;
    std::string name;
    bool available;
    std::optional<TransformComponent> transform;
    std::optional<SpriteComponent> sprite;
    std::optional<AnimationComponent> animation;
    std::optional<VelocityComponent> velocity;

    EntityTemplate() : available(false) {};
};

class GameData
{
private:
    static std::unique_ptr<GameData> instance;
    GameData();
    static GameData* createInstance();

    //Lưu dữ liệu map và entities load từ json
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

    // Truy cập dữ liệu
    const std::unordered_map<std::string, MapData>& getMaps() const;
    const std::unordered_map<std::string, std::unordered_map<std::string, EntityTemplate>>& getEntityTemplates() const;

    // Lấy MapData bằng name
    const MapData* getMap(const std::string& name) const;

    // Sửa dữ liệu (chưa dùng)
    void setMapAvailable(const std::string& name, bool available);

    void setSelectedCharacter(const std::string& characterName);
    std::string getSelectedCharacter() const;
    void setSelectedMap(const std::string& mapName);
    std::string getSelectedMap() const;
};

#endif  // __GAMEDATA_H__
