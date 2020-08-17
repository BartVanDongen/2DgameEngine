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
		ECS::Component* subComponent2 = new ECS::Sprite(subObject, "test2");
		ECS::Component* subComponent3 = new ECS::Animation(subObject, "test2");
		ECS::Component* Component1 = new ECS::Transform(entity, 20.0f, 20.0f, 1.0f, 2.0f);
		ECS::Component* Component2 = new ECS::Sprite(entity, "test");

		subObject->addComponent(subComponent1);
		subObject->addComponent(subComponent2);
		subObject->addComponent(subComponent3);

		entity->addComponent(Component1);
		entity->addComponent(Component2);
		entity->addSubObject(subObject);
		
		entityList.push_back(entity);
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		ECS::Renderer(this);
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