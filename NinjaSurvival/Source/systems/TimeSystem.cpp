#include "TimeSystem.h"
#include "SystemManager.h"
#include "axmol.h"
TimeSystem::TimeSystem() {}

void TimeSystem::init()
{
    uiLayer = SystemManager::getInstance()->getSceneLayer();
    if (!uiLayer)
    {
        AXLOG("Error: uiLayer is null in TimeSystem::init");
        return;
    }
    ax::Vec2 origin      = ax::Director::getInstance()->getVisibleOrigin();
    ax::Rect safeArea    = ax::Director::getInstance()->getSafeAreaRect();
    ax::Vec2 safeOrigin  = safeArea.origin;
    auto visibleSize     = ax::Director::getInstance()->getVisibleSize();

    // Tạo label để hiển thị thời gian
    timerLabel = ax::Label::createWithTTF("00:00", "fonts/Pixelpurl-0vBPP.ttf", 24);
    timerLabel->setPosition(ax::Vec2(safeOrigin.x + safeArea.size.width / 2,
                                     safeOrigin.y + safeArea.size.height - 20));  // Xem lại vị trí theo UI
    uiLayer->addChild(timerLabel,5); //Gán vào uiLayer
    timerLabel->setGlobalZOrder(2);

}

void TimeSystem::update(float dt)
{
    elapsedTime += dt;

    // Tính phút và giây
    int minutes = static_cast<int>(elapsedTime) / 60;
    int seconds = static_cast<int>(elapsedTime) % 60;

    // Cập nhật text cho label
    //Dùng mảng char cho đỡ tốn dung lượng
    char timeStr[6]; // Minutes sẽ làm tràn bộ nhớ nếu kết quả >= 100
    // %02d in ra số nguyên d với ít nhất 2 chữ số (bổ sung số 0 nếu thiếu)
    sprintf(timeStr, "%02d:%02d", minutes, seconds);
    timerLabel->setString(timeStr); 
}
