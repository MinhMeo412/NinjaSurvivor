#include "CameraSystem.h"

#include "SystemManager.h"
#include "SpawnSystem.h"

CameraSystem::CameraSystem(EntityManager& em,
                           ComponentManager<TransformComponent>& tm)
    : entityManager(em), transformMgr(tm) {}

void CameraSystem::init()
{
    scene = SystemManager::getInstance()->getCurrentScene();
    if (!scene)
    {
        AXLOG("Error: No scene available for CameraSystem");
        return;
    }

    camera = scene->getDefaultCamera();
    if (!camera)
    {
        AXLOG("Error: No default camera found in scene");
        return;
    }

    auto spawnSystem = SystemManager::getInstance()->getSystem<SpawnSystem>();
    if (spawnSystem)
    {
        playerEntity = spawnSystem->getPlayerEntity();
        AXLOG("CameraSystem: Tracking player entity %u", playerEntity);

        // Đặt vị trí ban đầu của camera và target
        if (auto transform = transformMgr.getComponent(playerEntity))
        {
            cameraTarget = ax::Vec2(transform->x, transform->y);
            camera->setPosition(cameraTarget);
            AXLOG("Camera firstzzzzzzzzzzzzzzzzzzzzzzz moved to x: %f, y: %f", cameraTarget.x, cameraTarget.y);
        }
    }
    else
    {
        AXLOG("Warning: SpawnSystem not found, assuming player is entity 0");
    }
}

void CameraSystem::update(float dt)
{
    if (!camera || !scene)
    {
        AXLOG("Error: Camera or scene not initialized in CameraSystem");
        return;
    }

    // Lấy vị trí trong TransformComponent của player 
    if (auto transform = transformMgr.getComponent(playerEntity))
    {
        // Cập nhật vị trí mục tiêu của camera
        cameraTarget = ax::Vec2(transform->x, transform->y);

        // Lấy vị trí hiện tại của camera
        ax::Vec2 currentPos = camera->getPosition();

        // Tính toán vị trí mới bằng lerp (di chuyển mượt mà)
        float lerpFactor = 5.0f * dt;  // Điều chỉnh tốc độ lerp (5.0f là ví dụ, có thể tăng/giảm)
        ax::Vec2 newPos      = currentPos + (cameraTarget - currentPos) * lerpFactor;

        // Cập nhật vị trí camera
        camera->setPosition(newPos);
        AXLOG("Camera moved to x: %f, y: %f (target x: %f, y: %f)", newPos.x, newPos.y, cameraTarget.x, cameraTarget.y);
    }
}

