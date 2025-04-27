#include "JoystickSystem.h"
#include "SystemManager.h"

using namespace ax;

JoystickSystem::JoystickSystem() {}

void JoystickSystem::init()
{
    // Tạo listener cho bàn phím
    auto listener = EventListenerKeyboard::create();

    // Khi nhấn phím
    listener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        switch (keyCode)
        {
        case EventKeyboard::KeyCode::KEY_UP_ARROW:
            isKeyUpPressed = true;
            break;
        case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
            isKeyDownPressed = true;
            break;
        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            isKeyLeftPressed = true;
            break;
        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            isKeyRightPressed = true;
            break;
        default:
            break;
        }
    };

    // Khi thả phím
    listener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event) {
        switch (keyCode)
        {
        case EventKeyboard::KeyCode::KEY_UP_ARROW:
            isKeyUpPressed = false;
            break;
        case EventKeyboard::KeyCode::KEY_DOWN_ARROW:
            isKeyDownPressed = false;
            break;
        case EventKeyboard::KeyCode::KEY_LEFT_ARROW:
            isKeyLeftPressed = false;
            break;
        case EventKeyboard::KeyCode::KEY_RIGHT_ARROW:
            isKeyRightPressed = false;
            break;
        default:
            break;
        }
    };

    Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(
        listener, SystemManager::getInstance()->getSceneLayer());
}

void JoystickSystem::update(float dt)
{
    // Tính toán hướng dựa trên trạng thái phím
    direction = Vec2::ZERO;

    // Xử lý trục X
    if (isKeyRightPressed && !isKeyLeftPressed)
    {
        direction.x = 1.0f;
    }
    else if (isKeyLeftPressed && !isKeyRightPressed)
    {
        direction.x = -1.0f;
    }

    // Xử lý trục Y
    if (isKeyUpPressed && !isKeyDownPressed)
    {
        direction.y = 1.0f;
    }
    else if (isKeyDownPressed && !isKeyUpPressed)
    {
        direction.y = -1.0f;
    }

    // Chuẩn hóa vector hướng để đảm bảo độ dài = 1 khi di chuyển chéo
    if (direction != Vec2::ZERO)
    {
        direction.normalize();
    }
}

Vec2 JoystickSystem::getDirection() const
{
    return direction;
}
