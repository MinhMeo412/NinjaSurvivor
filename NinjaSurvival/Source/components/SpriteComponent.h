#ifndef __SPRITE_COMPONENT_H__
#define __SPRITE_COMPONENT_H__

#include "axmol.h"


struct SpriteComponent
{
    ax::RefPtr<ax::Sprite> gameSceneFrame = nullptr;
    std::string plist;
    std::string gameSceneFrameName;
    ax::SpriteBatchNode* batchNode = nullptr;  // Batch node để tối ưu render (không dùng cho player)

    ax::GLProgramState* glProgramState = nullptr;  // Lưu trạng thái shader

    SpriteComponent() = default;
    SpriteComponent(const std::string& gameSceneFname, const std::string& plistFile)
        : gameSceneFrameName(gameSceneFname), plist(plistFile) {}
    
    void initializeSprite()
    {
        if (!gameSceneFrame && !gameSceneFrameName.empty() && !plist.empty())
        {
            // Load plist vào cache nếu chưa load
            if (!ax::SpriteFrameCache::getInstance()->isSpriteFramesWithFileLoaded(plist))
            {
                ax::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plist);
            }

            auto spriteFrame = ax::SpriteFrameCache::getInstance()->getSpriteFrameByName(gameSceneFrameName);

            if (spriteFrame)
            {
                gameSceneFrame = ax::Sprite::createWithSpriteFrame(spriteFrame);
            }
            else
            {
                AXLOG("Warning: Sprite frame %s not found in plist %s", gameSceneFrameName.c_str(), plist.c_str());
            }
        }
    }

    void setBatchNode(ax::SpriteBatchNode* node)
    { //Thực ra có thể tạo function này bên render nhưng lười
        batchNode = node;
        if (gameSceneFrame && batchNode && gameSceneFrame->getParent() != batchNode)
        {
            gameSceneFrame->removeFromParent();
            batchNode->addChild(gameSceneFrame);
        }
    }
};

struct AnimationComponent
{
    std::unordered_map<std::string,std::vector<std::string>> frames; // Danh sách frame cho từng trạng thái
    float frameDuration                 = 0.2f;// Thời gian mỗi frame

    std::string currentState;               // Trạng thái hiện tại
    ax::Action* currentAction = nullptr;    // Hành động animation hiện tại

    static const int ACTION_TAG_LEFT  = 1;  // Tag cho animation trái
    static const int ACTION_TAG_RIGHT = 2;  // Tag cho animation phải
    static const int ACTION_TAG_UP    = 3;  // Tag cho animation lên
    static const int ACTION_TAG_DOWN  = 4;  // Tag cho animation xuống
    static const int ACTION_TAG_IDLE  = 5;  // Tag cho animation idle

    AnimationComponent() = default;
    AnimationComponent(const std::unordered_map<std::string, std::vector<std::string>>& frameMap, float duration)
        : frames(frameMap), frameDuration(duration) {}

    void initializeAnimations()
    {
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
