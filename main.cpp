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
		for (int i = 0; i < 50; i++)
		{
			ECS::GameObject* entity = new ECS::GameObject();

			ECS::Component* component1 = new ECS::Transform(entity, (float)random(10, 240), (float)random(10, 240));
			entity->addComponent(component1);
			ECS::Component* component2 = new ECS::Sprite(entity, "test2");
			entity->addComponent(component2);
			ECS::Component* component3 = new ECS::Animation(entity, "test2");
			entity->addComponent(component3);
			ECS::Component* component4 = new ECS::BoxCollider(entity);
			entity->addComponent(component4);
			entityList.push_back(entity);
		}
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		ECS::Renderer(this);
		ECS::collision();
		return true;
	}

	int random(int aMin, int aMax)
	{
		return rand() % (aMax - aMin) + aMin;
	}

	vector<ECS::GameObject> entityList;
};

int main()
{
	Game game;
	if (game.Construct(256, 256, 2, 2))
		game.Start();
	return 0;
}