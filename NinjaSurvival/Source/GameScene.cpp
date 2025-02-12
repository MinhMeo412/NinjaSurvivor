#include "Utils.h"
#include "GameScene.h"
#include "MainScene.h"
#include "Character.h"
#include "Joystick.h"

using namespace ax;

bool GameScene::init()
{
	if (!SceneBase::init())
	{
		return false;
	}

	//UI Layer
	UILayer = Layer::create();
	UILayer->setAnchorPoint(Vec2(1, 1));
	this->addChild(UILayer, 10);

	//Border
	auto drawNode = DrawNode::create();
	drawNode->setPosition(Vec2(0, 0));
	UILayer->addChild(drawNode, 10);
	drawNode->drawRect(safeArea.origin + Vec2(1, 1), safeArea.origin + safeArea.size, Color4B::BLUE);


	//Close item
	auto closeItem = MenuItemImage::create("CloseNormal.png", "CloseSelected.png", AX_CALLBACK_1(GameScene::goToMainScene, this));

	if (closeItem == nullptr || closeItem->getContentSize().width <= 0 || closeItem->getContentSize().height <= 0)
	{
		Utils::problemLoading("'CloseNormal.png' and 'CloseSelected.png'");
	}
	else
	{
		float x = safeOrigin.x + safeArea.size.width - closeItem->getContentSize().width / 2;
		float y = safeOrigin.y + closeItem->getContentSize().height / 2;
		closeItem->setPosition(Vec2(x, y));
	}

	//Menu UI
	auto menu = Menu::create(closeItem, NULL);
	menu->setPosition(Vec2::ZERO);
	UILayer->addChild(menu, 10);    


    //Camera
    camera = getDefaultCamera();


	//Tiled map
	auto map = TMXTiledMap::create("Map.tmx");
	map->setAnchorPoint(Vec2(0, 0));
	map->setPosition(Vec2(0, 0));
	this->addChild(map, 2);

	//Character
	character = Character::create("character.plist", "./character_down0");
	character->setPosition(200, 200);
	character->setScale(3.0f);
	this->addChild(character, 3);

    // Joystick
    joystick = Joystick::create();
    this->addChild(joystick,4);

	// scheduleUpdate() is required to ensure update(float) is called on every loop
	scheduleUpdate();

	return true;
}





void GameScene::update(float delta)
{
    Vec2 direction = joystick->getDirection();
	character->update(direction,delta);
    
	camera->setPosition(character->getPosition());

	UILayer->setPosition(Vec2(camera->getPosition().x - visibleSize.width / 2, camera->getPosition().y - visibleSize.height / 2));

    joystick->setPosition(character->getPosition().x - visibleSize.width / 2, character->getPosition().y - visibleSize.height / 2);
}

void GameScene::menuCloseCallback(ax::Object* sender)
{
	_director->popScene();
}

void GameScene::goToMainScene(ax::Object* sender)
{
	auto scene = utils::createInstance<MainScene>();

	Director::getInstance()->replaceScene(TransitionFade::create(0.5f, scene));
}
