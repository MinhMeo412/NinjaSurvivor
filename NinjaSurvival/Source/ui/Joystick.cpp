#include "Joystick.h"

using namespace ax;

// on "init" you need to initialize your instance
Joystick* Joystick::create()
{
    Joystick* joystick = new (std::nothrow) Joystick();
    if (joystick && joystick->init())
    {
        joystick->autorelease();
        return joystick;
    }
    delete joystick;
    return nullptr;
}

bool Joystick::init()
{
    if (!Node::init())
    {
        return false;
    }

    _base  = Sprite::create("Joystick.png");
    _thumb = Sprite::create("Handle.png");

    if (!_base || !_thumb)
    {
        AXLOG("ERROR: Joystick image not exist");
        return false;
    }

    _base->setOpacity(128);
    _thumb->setOpacity(128);

    this->addChild(_base);
    this->addChild(_thumb);

    auto visibleSize = _director->getVisibleSize();

    _base->setPosition(Vec2(visibleSize.width / 2,visibleSize.height / 2 - 200));
    _thumb->setPosition(_base->getPosition());

    auto touchListener            = EventListenerTouchAllAtOnce::create();
    touchListener->onTouchesBegan = AX_CALLBACK_2(Joystick::onTouchesBegan, this);
    touchListener->onTouchesMoved = AX_CALLBACK_2(Joystick::onTouchesMoved, this);
    touchListener->onTouchesEnded = AX_CALLBACK_2(Joystick::onTouchesEnded, this);
    _eventDispatcher->addEventListenerWithSceneGraphPriority(touchListener, this);

    return true;
}

 void Joystick::onTouchesBegan(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
	for (auto&& t : touches)
	{
        Vec2 location = t->getLocation();
        if (_base->getBoundingBox().containsPoint(location))
        {
            _isTouching = true;
            _thumb->setPosition(location);
        }
	    AXLOG("onTouchesBegan detected, X:%f  Y:%f", t->getLocation().x, t->getLocation().y);
	}
 }

 void Joystick::onTouchesMoved(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
    //if (!_isTouching)
    //    return;
	for (auto&& t : touches)
	{
            Vec2 location = t->getLocation();
        Vec2 offset   = location - _base->getPosition();

        float maxRadius = _base->getContentSize().width * 0.5f;
        if (offset.length() > maxRadius)
        {
            offset.normalize();
            offset *= maxRadius;
        }

        _thumb->setPosition(_base->getPosition() + offset);
        _direction = offset.getNormalized();  // Chuẩn hóa vector hướng
		AXLOG("onTouchesMoved detected, X:%f  Y:%f", t->getLocation().x, t->getLocation().y);
	}
 }

 void Joystick::onTouchesEnded(const std::vector<ax::Touch*>& touches, ax::Event* event)
{
	for (auto&& t : touches)
	{
        _isTouching = false;
        _thumb->setPosition(_base->getPosition());  // Reset thumb về giữa
        _direction = Vec2::ZERO;                    // Reset hướng
		AXLOG("onTouchesEnded detected, X:%f  Y:%f", t->getLocation().x, t->getLocation().y);
	}
 }

 Vec2 Joystick::getDirection() const
 {
     return _direction;
 }
