#ifndef __AUDIO_MANAGER_H__
#define __AUDIO_MANAGER_H__

#include "axmol.h"
#include <unordered_map>
#include <audio/AudioEngine.h>
#include <vector>

class AudioManager
{
public:
    // Singleton pattern để đảm bảo chỉ có một instance duy nhất
    static AudioManager* getInstance();

    // Hủy instance
    static void destroyInstance();

    // Khởi tạo và preload các file âm thanh
    void init();

    // Phát một âm thanh với ID
    int playSound(const std::string& soundId, bool loop = false, float volume = 1.0f, const std::string& category = "");

    // Dừng âm thanh theo audioID
    void stopSound(const std::string& category = "");

    // Tạm dừng âm thanh
    void pauseSound(const std::string& category = "");

    void pauseAllSounds();

    void resumeAllSounds();

    // Tiếp tục âm thanh
    void resumeSound(const std::string& category = "");

    // Dừng tất cả âm thanh
    void stopAllSounds();

    // Điều chỉnh âm lượng toàn cục
    void setGlobalVolume(float volume);

    // Lấy âm lượng toàn cục
    float getGlobalVolume() const;

    // Lấy audioID của một category cụ thể
    int getSoundID(const std::string& category) const;

private:
    AudioManager();
    ~AudioManager();

    static AudioManager* s_instance;

    // Map lưu trữ các file âm thanh đã preload với key là soundId
    std::unordered_map<std::string, std::string> _soundFiles;
    std::unordered_map<std::string, int> _categoryToAudioID;
    std::vector<int> _activeAudioIDs;
    // Âm lượng toàn cục
    float _globalVolume;
};

#endif  // __AUDIO_MANAGER_H__
