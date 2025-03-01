#include "JoystickSystem.h"
#include "SystemManager.h"

using namespace ax;

JoystickSystem::JoystickSystem() {}

void JoystickSystem::init()
{
    auto systemManager = SystemManager::getInstance();
    parentScene        = systemManager->getCurrentScene();

    if (!parentScene)
    {
        AXLOG("Error: No scene set for JoystickSystem");
        return;
    }

    joystickBase = Sprite::create("Joystick.png");
    joystickBase->setPosition(Vec2(640, 300));
    parentScene->addChild(joystickBase,10);

    joystickHandle = Sprite::create("Handle.png");
    joystickHandle->setPosition(joystickBase->getPosition());
    parentScene->addChild(joystickHandle,10);

    auto listener          = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        Vec2 touchLocation = touch->getLocation();
        Vec2 baseLocation  = joystickBase->getPosition();
        float distance     = touchLocation.distance(baseLocation);

        if (distance <= maxDistance * 2)
        {
            isDragging = true;
            return true;
        }
        return false;
    };

    listener->onTouchMoved = [this](Touch* touch, Event* event) {
        if (isDragging)
        {
            Vec2 touchLocation = touch->getLocation();
            Vec2 baseLocation  = joystickBase->getPosition();
            Vec2 delta         = touchLocation - baseLocation;

            float distance = delta.length();
            if (distance > maxDistance)
            {
                delta = delta.getNormalized() * maxDistance;
            }

            joystickHandle->setPosition(baseLocation + delta);

            //Quy đổi ra hệ Vec2 của hướng đi ??
            velocity.x = delta.x / maxDistance;
            velocity.y = delta.y / maxDistance;

            AXLOG(" Joystick Velocity : x = % f, y = % f ", velocity.x, velocity.y);
        }
    };

    listener->onTouchEnded = [this](Touch* touch, Event* event) {
        isDragging = false;
        joystickHandle->setPosition(joystickBase->getPosition());
        velocity.x = 0.0f;
        velocity.y = 0.0f;
    };

    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, joystickHandle);
}

void JoystickSystem::update(float dt)
{}

VelocityComponent JoystickSystem::getVelocity() const
{
    return velocity;
}
