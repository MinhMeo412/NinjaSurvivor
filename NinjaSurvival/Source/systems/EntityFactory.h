#ifndef __ENTITY_FACTORY_H__
#define __ENTITY_FACTORY_H__

#include "entities/Entity.h"
#include "components/ComponentManager.h"
#include "components/Components.h"
#include "entities/EntityManager.h"
#include "axmol.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <cstdio>

class EntityFactory
{
//public:
//    EntityFactory(EntityManager& entityMgr,
//                  ComponentManager<PositionComponent>& posMgr,
//                  ComponentManager<VelocityComponent>& velMgr,
//                  ComponentManager<SpriteComponent>& spriteMgr,
//                  ComponentManager<AnimationComponent>& animMgr);
//    Entity createEntityFromJson(const rapidjson::Value& entityData, ax::Scene* scene);
//
//private:
//    EntityManager& entityMgr;
//    ComponentManager<PositionComponent>& positionMgr;
//    ComponentManager<VelocityComponent>& velocityMgr;
//    ComponentManager<SpriteComponent>& spriteMgr;
//    ComponentManager<AnimationComponent>& animMgr;
//
//    bool loadPlistAndCreateAnimations(const std::string& plistPath,
//                                      int frameCount,
//                                      float frameDuration,
//                                      ax::Sprite*& sprite,
//                                      ax::Scene* scene);
//    ax::Animation* createAnimation(const std::string& type,
//                                   int id,
//                                   const std::string& direction,
//                                   int frameCount,
//                                   float delay);
};

#endif  // ENTITY_FACTORY_H
