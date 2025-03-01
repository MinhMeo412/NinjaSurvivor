#include "EntityFactory.h"
#include <cstdio>

//EntityFactory::EntityFactory(EntityManager& entityMgr,
//                             ComponentManager<PositionComponent>& posMgr,
//                             ComponentManager<VelocityComponent>& velMgr,
//                             ComponentManager<SpriteComponent>& spriteMgr,
//                             ComponentManager<AnimationComponent>& animMgr)
//    : entityMgr(entityMgr), positionMgr(posMgr), velocityMgr(velMgr), spriteMgr(spriteMgr), animMgr(animMgr)
//{}
//
//Entity EntityFactory::createEntityFromJson(const rapidjson::Value& entityData, ax::Scene* scene)
//{
//    Entity entity = entityMgr.createEntity();
//
//    // Lấy dữ liệu từ JSON (rapidjson)
//    const rapidjson::Value& typeValue = entityData["type"];
//    std::string type       = typeValue.GetString();
//
//    const rapidjson::Value& idValue = entityData["id"];
//    int id                          = idValue.GetInt();
//
//    if (type == "player")
//    {
//        // PositionComponent
//        if (entityData.HasMember("components") && entityData["components"].HasMember("position"))
//        {
//            const rapidjson::Value& posData = entityData["components"]["position"];
//            positionMgr.addComponent(entity, PositionComponent{posData["x"].GetFloat(), posData["y"].GetFloat()});
//        }
//
//        // VelocityComponent
//        if (entityData.HasMember("components") && entityData["components"].HasMember("velocity"))
//        {
//            const rapidjson::Value& velData = entityData["components"]["velocity"];
//            velocityMgr.addComponent(entity, VelocityComponent{velData["vx"].GetFloat(), velData["vy"].GetFloat()});
//        }
//
//        // AnimationComponent (sử dụng .plist)
//        if (entityData.HasMember("components") && entityData["components"].HasMember("animation"))
//        {
//            const rapidjson::Value& animData = entityData["components"]["animation"];
//            if (animData.HasMember("plist") && animData.HasMember("frameCount") && animData.HasMember("frameDuration"))
//            {
//                std::string plistPath = animData["plist"].GetString();
//                int frameCount        = animData["frameCount"].GetInt();
//                float frameDuration   = animData["frameDuration"].GetFloat();
//
//                ax::Sprite* sprite = nullptr;
//                if (loadPlistAndCreateAnimations(plistPath, frameCount, frameDuration, sprite, scene))
//                {
//                    if (sprite)
//                    {
//                        PositionComponent* pos = positionMgr.getComponent(entity);
//                        if (pos)
//                        {
//                            sprite->setPosition(pos->x, pos->y);
//                        }
//                        else
//                        {
//                            AXLOG("No PositionComponent for entity %u, setting default position (320, 240)", entity);
//                            sprite->setPosition(320, 240);  // Vị trí mặc định nếu không có PositionComponent
//                        }
//                        sprite->retain();  // Giữ tham chiếu để tránh giải phóng sớm
//                        spriteMgr.addComponent(entity, SpriteComponent{sprite});
//                        AXLOG("Added SpriteComponent for entity %u with sprite %p", entity, sprite);
//                    }
//                    else
//                    {
//                        AXLOG("Sprite is nullptr for entity %u, skipping SpriteComponent", entity);
//                    }
//
//                    // Tạo AnimationComponent với các animation từ AnimationCache
//                    std::string downAnim  = "down";
//                    std::string upAnim    = "up";
//                    std::string leftAnim  = "left";
//                    std::string rightAnim = "right";
//
//                    // Tạo và lưu animation cho từng hướng
//                    ax::Animation* downAnimation = createAnimation(type, id, "down", frameCount, frameDuration);
//                    if (downAnimation)
//                    {
//                        ax::AnimationCache::getInstance()->addAnimation(downAnimation, downAnim);
//                    }   
//
//                    ax::Animation* upAnimation = createAnimation(type, id, "up", frameCount, frameDuration);
//                    if (upAnimation)
//                    {
//                        ax::AnimationCache::getInstance()->addAnimation(upAnimation, upAnim);
//                    }
//
//                    ax::Animation* leftAnimation = createAnimation(type, id, "left", frameCount, frameDuration);
//                    if (leftAnimation)
//                    {
//                        ax::AnimationCache::getInstance()->addAnimation(leftAnimation, leftAnim);
//                    }
//
//                    ax::Animation* rightAnimation = createAnimation(type, id, "right", frameCount, frameDuration);
//                    if (rightAnimation)
//                    {
//                        ax::AnimationCache::getInstance()->addAnimation(rightAnimation, rightAnim);
//                    }
//
//                    // Thêm AnimationComponent với các tên animation
//                    animMgr.addComponent(entity, AnimationComponent(downAnim, upAnim, leftAnim, rightAnim));
//                    AXLOG("Added AnimationComponent for entity %u with animations: down=%s, up=%s, left=%s, right=%s",
//                          entity, downAnim.c_str(), upAnim.c_str(), leftAnim.c_str(), rightAnim.c_str());
//                }
//                else
//                {
//                    AXLOG("Failed to load plist or create animations for entity %u", entity);
//                }
//            }
//            else
//            {
//                AXLOG("Missing required animation fields for entity %u", entity);
//            }
//        }
//    }
//    return entity;
//}
//
//
//bool EntityFactory::loadPlistAndCreateAnimations(const std::string& plistPath,
//                                                 int frameCount,
//                                                 float frameDuration,
//                                                 ax::Sprite*& sprite,
//                                                 ax::Scene* scene)
//{
//    // Kiểm tra và tải .plist vào SpriteFrameCache
//    AXLOG("Loading plist: %s", plistPath.c_str());
//    ax::SpriteFrameCache::getInstance()->addSpriteFramesWithFile(plistPath.c_str());
//
//    // Tạo sprite ban đầu từ frame đầu tiên (giả sử frame đầu tiên là default, ví dụ: player1_down0)
//    std::string firstFrame = ax::StringUtils::format("./%s_down0", "character");  // Giả sử type="player", id=1
//    sprite                 = ax::Sprite::createWithSpriteFrameName(firstFrame.c_str());
//    if (!sprite)
//    {
//        AXLOG("Failed to create sprite from first frame %s in %s", firstFrame.c_str(), plistPath.c_str());
//        return false;
//    }
//
//    // Thêm sprite vào scene để kiểm tra
//    PositionComponent* pos = nullptr;  // Giả sử bạn có thể lấy từ entity, nhưng tạm thời đặt vị trí mặc định
//    sprite->setPosition(320, 240);  // Vị trí tạm để kiểm tra
//    sprite->retain();               // Giữ tham chiếu cho sprite
//    scene->addChild(sprite);
//
//    return true;
//}
//
//ax::Animation* EntityFactory::createAnimation(const std::string& type,
//                                              int id,
//                                              const std::string& direction,
//                                              int frameCount,
//                                              float delay)
//{
//    ax::Vector<ax::SpriteFrame*> frames;
//    for (int i = 0; i < frameCount; ++i)
//    {
//        std::string frameName  = ax::StringUtils::format("./%s_%s%d", type.c_str(), direction.c_str(), i);
//        ax::SpriteFrame* frame = ax::SpriteFrameCache::getInstance()->getSpriteFrameByName(frameName.c_str());
//        if (frame)
//        {
//            frames.pushBack(frame);
//            AXLOG("Loaded frame for animation: %s", frameName.c_str());
//        }
//        else
//        {
//            AXLOG("Frame not found: %s", frameName.c_str());
//        }
//    }
//
//    if (frames.empty())
//    {
//        AXLOG("No frames found for animation: %s%d_%s", type.c_str(), id, direction.c_str());
//        return nullptr;  // Trả về nullptr nếu không có frame nào
//    }
//
//    return ax::Animation::createWithSpriteFrames(frames, delay);
//}
