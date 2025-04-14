#include "AudioManager.h"
#include "audio/AudioEngine.h"
USING_NS_AX;

AudioManager* AudioManager::s_instance = nullptr;

AudioManager* AudioManager::getInstance()
{
    if (!s_instance)
    {
        s_instance = new AudioManager();
    }
    return s_instance;
}

void AudioManager::destroyInstance()
{
    if (s_instance)
    {
        delete s_instance;
        s_instance = nullptr;
    }
}

AudioManager::AudioManager() : _globalVolume(1.0f) {}

AudioManager::~AudioManager()
{
    stopAllSounds();
    AudioEngine::end();  // Dọn dẹp AudioEngine khi hủy
}

void AudioManager::init()
{
    // Preload các file âm thanh khi khởi động
    _soundFiles["background_music"] = "Musics/12 - Temple.ogg";
    _soundFiles["exp"]     = "Sounds/Game/Gold2.wav";
    _soundFiles["explosion"]        = "Sounds/Game/Explosion.wav";
    _soundFiles["coin"]                 = "Sounds/Game/Coin.wav";
    _soundFiles["button_click"]                  = "Sounds/Game/Bonus.wav";
    _soundFiles["gamebackground_music"] = "Musics/17 - Fight.ogg";

    // Preload tất cả âm thanh trong map
    for (const auto& pair : _soundFiles)
    {
        AudioEngine::preload(pair.second);
    }

    // Đặt âm lượng mặc định
    //AudioEngine::setDefaultVolume(_globalVolume);
}

int AudioManager::playSound(const std::string& soundId, bool loop, float volume, const std::string& category)
{
    auto it = _soundFiles.find(soundId);
    if (it != _soundFiles.end())
    {
        float adjustedVolume = volume * _globalVolume;
        int audioID          = AudioEngine::play2d(it->second, loop, adjustedVolume);
        if (audioID != AudioEngine::INVALID_AUDIO_ID)
        {
            _activeAudioIDs.push_back(audioID);  // Lưu audioID vào danh sách
            if (!category.empty())
            {
                _categoryToAudioID[category] = audioID;  // Lưu audioID theo category
            }
        }
        return audioID;
    }
    return AudioEngine::INVALID_AUDIO_ID;  
}

void AudioManager::stopSound(const std::string& category)
{
    if (!category.empty())
    {
        auto it = _categoryToAudioID.find(category);
        if (it != _categoryToAudioID.end())
        {
            AudioEngine::stop(it->second);
            _activeAudioIDs.erase(std::remove(_activeAudioIDs.begin(), _activeAudioIDs.end(), it->second),
                                  _activeAudioIDs.end());
            _categoryToAudioID.erase(it);
        }
    }
}

void AudioManager::pauseSound(const std::string& category)
{
    if (!category.empty())
    {
        auto it = _categoryToAudioID.find(category);
        if (it != _categoryToAudioID.end())
        {
            AudioEngine::pause(it->second);
        }
    }
}

void AudioManager::resumeSound(const std::string& category)
{
    if (!category.empty())
    {
        auto it = _categoryToAudioID.find(category);
        if (it != _categoryToAudioID.end())
        {
            AudioEngine::resume(it->second);
        }
    }
}

void AudioManager::stopAllSounds()
{
    AudioEngine::stopAll();
    _activeAudioIDs.clear();
    _categoryToAudioID.clear();
}

void AudioManager::pauseAllSounds()
{
    AudioEngine::pauseAll();
}

void AudioManager::resumeAllSounds()
{
    AudioEngine::resumeAll();
}

void AudioManager::setGlobalVolume(float volume)
{
    _globalVolume = std::max(0.0f, std::min(1.0f, volume));
    // Cập nhật âm lượng cho tất cả âm thanh đang phát
    for (int audioID : _activeAudioIDs)
    {
        AudioEngine::setVolume(audioID, _globalVolume);
    }
}

float AudioManager::getGlobalVolume() const
{
    return _globalVolume;
}

int AudioManager::getSoundID(const std::string& category) const
{
    auto it = _categoryToAudioID.find(category);
    if (it != _categoryToAudioID.end())
    {
        return it->second;
    }
    return AudioEngine::INVALID_AUDIO_ID;
}
