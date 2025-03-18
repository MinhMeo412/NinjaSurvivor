#ifndef __CAMERA_SYSTEM_H__
#define __CAMERA_SYSTEM_H__

#include "System.h"
#include "components/ComponentManager.h"
#include "entities/EntityManager.h"
#include "axmol.h"

class CameraSystem : public System
{
public:
    CameraSystem(EntityManager& em,
                 ComponentManager<TransformComponent>& tm); 

    void init() override;
    void update(float dt) override;

    ax::Camera* getCamera() const { return camera; }

private:
    EntityManager& entityManager;
    ComponentManager<TransformComponent>& transformMgr;

    ax::Camera* camera = nullptr;
    ax::Scene* scene   = nullptr;

    Entity playerEntity = 0;
    ax::Vec2 cameraTarget; // Vị trí mục tiêu mà camera sẽ di chuyển tới
};
//Chưa có giới hạn ranh giới camera chỉ trong map (optional)



#endif  // __CAMERA_SYSTEM_H__
