#include "JoystickSystem.h"
#include "SystemManager.h"

using namespace ax;

JoystickSystem::JoystickSystem() {}

void JoystickSystem::init()
{
    parentLayer = SystemManager::getInstance()->getSceneLayer();

    if (!parentLayer)
    {
        AXLOG("Error: No layer set for JoystickSystem");
        return;
    }

    joystickBase = Sprite::create("Joystick.png");
    //joystickBase->setPosition(Vec2(640, 300)); //Không còn cố định vị trí
    joystickBase->setOpacity(128);
    joystickBase->setVisible(false);  //Ẩn joystick
    parentLayer->addChild(joystickBase, 10);
    joystickBase->setGlobalZOrder(9);

    joystickHandle = Sprite::create("Handle.png");
    //joystickHandle->setPosition(joystickBase->getPosition()); //Không còn cố định vị trí
    joystickHandle->setOpacity(128);
    joystickHandle->setVisible(false);  // Ẩn joystick
    parentLayer->addChild(joystickHandle, 10);
    joystickHandle->setGlobalZOrder(9);

    maxDistance = joystickBase->getContentSize().width / 2;

    //Tạo listenẻ
    auto listener          = EventListenerTouchOneByOne::create();

    //Khi chạm
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        if (isDragging) return false; // Nếu đã có chạm khác, không xử lý thêm

        Vec2 touchLocation = touch->getLocation();

        joystickBase->setPosition(touchLocation);
        joystickHandle->setPosition(touchLocation);
        joystickBase->setVisible(true);     // Hiện joystick
        joystickHandle->setVisible(true);  // Hiện handle
        isDragging = true;
        isVisible  = true;
        direction  = Vec2::ZERO;

        return true;
    };

    //Khi di chuyển
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

            // Trả về hướng đi (chuẩn hóa vector delta)
            if (distance > 0)  // Tránh chia cho 0
            {
                delta.normalize();
                direction = delta;  // Gán hướng đã chuẩn hóa
            }
            else
            {
                direction = Vec2::ZERO;  // Tại tâm, hướng = (0, 0)
            }
        }
    };

    //Khi kết thúc chạm
    listener->onTouchEnded = [this](Touch* touch, Event* event) {
        if (isDragging)
        {
        isDragging = false;
        isVisible  = false;
        joystickBase->setVisible(false);    // Ẩn joystick
        joystickHandle->setVisible(false);  // Ẩn handle
        direction = Vec2::ZERO;
        }
    };

    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, parentLayer);
}

void JoystickSystem::update(float dt) {}


Vec2 JoystickSystem::getDirection() const
{
    return direction;
}
