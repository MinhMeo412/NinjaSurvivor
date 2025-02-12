#include "AudioManager.h"
#include "audio/AudioEngine.h"

using namespace ax;

//AudioManager* AudioManager::_instance = nullptr;
//
//AudioManager::AudioManager() {}
//
//AudioManager::~AudioManager() {}
//
//AudioManager* AudioManager::getInstance()
//{
//    if (_instance == nullptr)
//    {
//        _instance = new AudioManager();
//    }
//    return _instance;
//}
//
//void AudioManager::playClickEffect()
//{
//    AudioEngine::play2d(CLICK_EFFECT, false, 1.0f);  // false: không lặp lại, 1.0f: âm lượng
//}
//
//void AudioManager::playHitEffect()
//{
//    AudioEngine::play2d(HIT_EFFECT, false, 1.0f);
//}
//
//void AudioManager::playEatEffect()
//{
//    AudioEngine::play2d(EAT_EFFECT, false, 1.0f);
//}
