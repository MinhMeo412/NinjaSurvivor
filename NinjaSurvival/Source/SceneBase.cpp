#include "SceneBase.h"
//#include "AudioManager.h"

using namespace ax;

bool SceneBase::init()
{
    if (!Scene::init())
    {
        return false;
    }

    visibleSize = _director->getVisibleSize();
    origin      = _director->getVisibleOrigin();
    safeArea    = _director->getSafeAreaRect();
    safeOrigin  = safeArea.origin;

    //For PC
    //setupMouseListener();

    return true;
}

//For PC
//void SceneBase::setupMouseListener()
//{
//    mouseListener = ax::EventListenerMouse::create();
//
//    mouseListener->onMouseDown = [this](ax::Event* event) {
//        auto mouseEvent = static_cast<ax::EventMouse*>(event);
//        this->onMouseDown(mouseEvent);                       
//    };
//
//    mouseListener->onMouseUp = [this](ax::Event* event) {
//        auto mouseEvent = static_cast<ax::EventMouse*>(event); 
//        this->onMouseUp(mouseEvent);                         
//    };
//
//    mouseListener->onMouseMove = [this](ax::Event* event) {
//        auto mouseEvent = static_cast<ax::EventMouse*>(event);
//        this->onMouseMove(mouseEvent);                      
//    };
//
//    mouseListener->onMouseScroll = [this](ax::Event* event) {
//        auto mouseEvent = static_cast<ax::EventMouse*>(event);
//        this->onMouseScroll(mouseEvent);                       
//    };
//
//    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(mouseListener, this);
//}
//
//void SceneBase::onMouseDown(Event* event)
//{
//    EventMouse* e = static_cast<EventMouse*>(event);
//    AXLOG("onMouseDown detected, Key: %d", static_cast<int>(e->getMouseButton()));
//}
//
//void SceneBase::onMouseUp(Event* event)
//{
//    EventMouse* e = static_cast<EventMouse*>(event);
//    AXLOG("onMouseUp detected, Key: %d", static_cast<int>(e->getMouseButton()));
//}
//
//void SceneBase::onMouseMove(Event* event)
//{
//    EventMouse* e = static_cast<EventMouse*>(event);
//    AXLOG("onMouseMove detected, X:%f  Y:%f", e->getCursorX(), e->getCursorY());
//}
//
//void SceneBase::onMouseScroll(Event* event)
//{
//    EventMouse* e = static_cast<EventMouse*>(event);
//    AXLOG("onMouseScroll detected, X:%f  Y:%f", e->getScrollX(), e->getScrollY());
//}

