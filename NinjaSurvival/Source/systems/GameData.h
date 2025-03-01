#ifndef __GAMEDATA_H__
#define __GAMEDATA_H__

#include "axmol.h"
#include <string>
#include <memory>  //unique ptr
#include <unordered_map>

struct MapData
{
    std::string tmxFile;
    std::string name;
    std::string sprite;
    bool available;

    MapData() : available(false) {};
};

class GameData
{
private:
    static std::unique_ptr<GameData> instance;

    GameData();
    static GameData* createInstance();

    std::unordered_map<std::string, MapData> maps;

    std::string readFileContent(const std::string& filename);

    // Parse JSON và lưu vào maps
    bool loadMapData(const std::string& jsonString);
public:
    ~GameData();
    static GameData* getInstance();

    bool loadMapDataFromFile(const std::string& filename);

    // Truy cập dữ liệu
    const std::unordered_map<std::string, MapData>& getMaps() const;
    const MapData* getMap(const std::string& name) const;  // Lấy MapData bằng name

    // Sửa dữ liệu (chưa dùng)
    void setMapAvailable(const std::string& name, bool available);
};

#endif  // __GAMEDATA_H__
