#ifndef __SPRITE_COMPONENT_H__
#define __SPRITE_COMPONENT_H__

#include "axmol.h"

struct SpriteComponent
{
    ax::RefPtr<ax::Sprite> frame = nullptr;
    ax::RefPtr<ax::Sprite> gameSceneFrame = nullptr;

    ax::DrawNode* debugDrawNode = nullptr;//Debug
    std::string filename;
    std::string gameSceneFilename;

    SpriteComponent() = default;
    SpriteComponent(const std::string& fname, const std::string& gameSceneFname)
        : filename(fname), gameSceneFilename(gameSceneFname) {}

    void initializeSprite()
    { 
        if (!frame && !filename.empty())
        {
            frame = ax::Sprite::create(filename);
        }
        if (!gameSceneFrame && !gameSceneFilename.empty())
        {
            gameSceneFrame = ax::Sprite::create(gameSceneFilename);
        }


    }
};

struct AnimationComponent
{
    std::string plist;
    std::unordered_map<std::string,std::vector<std::string>> frames;
    float frameDuration                 = 0.2f;

    std::string currentState;

    ax::Action* currentAction = nullptr;

    static const int ACTION_TAG_LEFT  = 1;  // Tag cho animation trái
    static const int ACTION_TAG_RIGHT = 2;  // Tag cho animation phải
    static const int ACTION_TAG_UP    = 3;  // Tag cho animation lên
    static const int ACTION_TAG_DOWN  = 4;  // Tag cho animation xuống
    static const int ACTION_TAG_IDLE  = 5;  // Tag cho animation idle

    AnimationComponent() = default;
    AnimationComponent(const std::string& plistFile,
                       const std::unordered_map<std::string, std::vector<std::string>>& frameMap,
                       float duration)
        : plist(plistFile), frames(frameMap), frameDuration(duration)
    {}

    void initializeAnimations()
    {
        //Kiểm tra plist nếu không trống thì lưu danh sách các frame trong plist
        if (!plist.empty())
        {
            ax::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist);
        }

        //Duyệt danh sách từng trạng thái animation và danh sách frame của animation đó
        for (const auto& [state, frameList] : frames)
        {
            //Nếu danh sách frame trống thì bỏ qua
            if (frameList.empty())
                continue;

            //Kiểm tra animation trong cache (nếu đã có thì bỏ qua)
            if (ax::AnimationCache::getInstance()->getAnimation(state))
            {
                continue;
            }

            //Tạo một animation
            auto anim = ax::Animation::create();
            for (const auto& frameName : frameList)
            {
                auto frame = ax::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName);
                if (frame)
                {
                    anim->addSpriteFrame(frame);
                }
                else
                {
                    AXLOG("Warning: Sprite frame %s not found", frameName.c_str());
                }
            }
            anim->setDelayPerUnit(frameDuration);

            //Thêm animation vào cache
            ax::AnimationCache::getInstance()->addAnimation(anim, state);
        }
    }
};

#endif  // __SPRITE_COMPONENT_H__
