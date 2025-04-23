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
    //Music
    _soundFiles["background_music"]         = "Sounds/Musics/12 - Temple.ogg";
    _soundFiles["gamebackground_music"]     = "Sounds/Musics/17 - Fight.ogg";
    _soundFiles["game_over"]                = "Sounds/Musics/GameOver.wav";

    //SFX
    _soundFiles["button_click"]             = "Sounds/GameSFX/ButtonClick.wav";
    _soundFiles["exp"]                      = "Sounds/GameSFX/Exp.wav";
    _soundFiles["bomb"]                     = "Sounds/GameSFX/Bomb.wav";
    _soundFiles["coin"]                     = "Sounds/GameSFX/Coin.wav";
    _soundFiles["magnet_heart"]             = "Sounds/GameSFX/Magnet.wav";
    _soundFiles["chest"]                    = "Sounds/GameSFX/Chest.wav";

    _soundFiles["get_hit"]                  = "Sounds/GameSFX/GetHit.wav";
    _soundFiles["hit"]                      = "Sounds/GameSFX/Hit.wav";

    _soundFiles["boss_alert"]               = "Sounds/GameSFX/BossAlert.wav";

    _soundFiles["sword"]                    = "Sounds/GameSFX/Sword.wav";
    _soundFiles["explosion_kunai"]          = "Sounds/GameSFX/Explosion_kunai.wav";
    _soundFiles["lightning_scroll"]         = "Sounds/GameSFX/LightningStrike.wav";


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
