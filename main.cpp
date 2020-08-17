#include "EntityComponentSystem.h"

class Game : public olc::PixelGameEngine
{
public:
	Game()
	{
		sAppName = "Game";
	}

	bool OnUserCreate() override
	{
		entity = new ECS::GameObject();
		subObject = new ECS::GameObject(entity);

		ECS::Component* subComponent1 = new ECS::Transform(subObject, 0.0f, 10.0f, 2.0f, 1.0f);
		subObject->addComponent(subComponent1);
		ECS::Component* subComponent2 = new ECS::Sprite(subObject, "test2");
		subObject->addComponent(subComponent2);
		ECS::Component* subComponent3 = new ECS::Animation(subObject, "test2");
		subObject->addComponent(subComponent3);
		ECS::Component* subComponent4 = new ECS::BoxCollider(subObject);
		subObject->addComponent(subComponent4);
		ECS::Component* component1 = new ECS::Transform(entity, 20.0f, 20.0f, 1.0f, 2.0f);
		entity->addComponent(component1);
		ECS::Component* component2 = new ECS::Sprite(entity, "test");
		entity->addComponent(component2);
		ECS::Component* component3 = new ECS::Animation(entity, "test");
		entity->addComponent(component3);
		ECS::Component* component4 = new ECS::BoxCollider(entity);
		entity->addComponent(component4);
		
		entity->addChild(subObject);
		
		entityList.push_back(entity);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		ECS::Renderer(this);
		ECS::collision();
		return true;
	}

	ECS::GameObject* entity;
	ECS::GameObject* subObject;
	vector<ECS::GameObject> entityList;
};

int main()
{
	Game game;
	if (game.Construct(256, 256, 2, 2))
		game.Start();
	return 0;
}