#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "json.hpp"
#include <map>
#include <fstream>
#pragma once

using namespace std;
using json = nlohmann::json;

namespace ECS
{
	struct Transform;
	struct Sprite;
	struct Animation;
	struct BoxCollider;
	class GameObject;

	vector<Transform*> transformList;
	vector<Sprite*> spriteList;
	vector<Animation*> animationList;
	vector<BoxCollider*> boxColliderList;

	struct Component
	{
		Component() {}
		string name = "component";
	};

	struct Transform : public Component
	{
		Transform(GameObject* aParent, int aX = 0.0f, int aY = 0.0f, float aScaleX = 1.0f, float aScaleY = 1.0f)
		{
			x = aX;
			y = aY;
			scaleX = aScaleX;
			scaleY = aScaleY;
			parent = aParent;
			transformList.push_back(this);
		}
		string name = "transform";
		float x;
		float y;
		float scaleX;
		float scaleY;
		GameObject* parent;
	};

	struct Sprite : public Component
	{
		Sprite(GameObject* aParent, string aTileSetName)
		{
			objectDecal = new olc::Decal(new olc::Sprite("assets/" + aTileSetName + ".png"));
			parent = aParent;
			spriteList.push_back(this);
		}
		olc::Decal* objectDecal;
		bool isActive = true;
		GameObject* parent;
	};

	struct Animation : public Component
	{
		Animation(GameObject* aParent, string aTileSetName)
		{
			ifstream ifslevel("assets/" + aTileSetName + ".json");
			json myJson = json::parse(ifslevel);
			tileSetSize = myJson["tileSetSize"].get<vector<int>>();
			tileSize = myJson["tileSize"].get<vector<int>>();
			animationMap = myJson["Animation"].get<map<string, vector<int>>>();
			parent = aParent;
			animationList.push_back(this);
		}
		vector<int> tileSetSize;
		vector<int> tileSize;
		int tile = 1;
		map<string, vector<int>> animationMap;
		GameObject* parent;
	};

	struct BoxCollider : public Component
	{
		// sprite size
		BoxCollider(GameObject* aParent)
		{
			parent = aParent;
			Animation* myAnimation = (Animation*)aParent->getComponentMap().find("sprite")->second;
			colliderSize = myAnimation->tileSize;
			boxColliderList.push_back(this);
		}

		// custom size
		BoxCollider(GameObject* aParent, vector<float> aPos, vector<int> aColliderSize)
		{
			parent = aParent;
			pos = aPos;
			colliderSize = aColliderSize;
			boxColliderList.push_back(this);
		}

		bool isActive = true;
		vector<float> pos = {0.0f, 0.0f};
		vector<int> colliderSize;
		vector<BoxCollider*> collidingWith;
		GameObject* parent;
	};

	// Entity
	class GameObject
	{
	public:
		GameObject(GameObject* aParent = NULL)
		{
			parent = aParent;
		}

		void addComponent(Component* aComponent)
		{
			string name = aComponent->name;
			componentMap.insert(pair<string, Component*>(name, aComponent));
		}

		void addSubObject(GameObject aGameObject)
		{
			aGameObject.hasParent = true;
			gameObjectMap.push_back(aGameObject);
		}

		GameObject* getParent()
		{
			return parent;
		}

		map<string, Component*> getComponentMap()
		{
			return componentMap;
		}

	private:
		bool hasParent = false;
		map<string, Component*> componentMap;
		vector<GameObject> gameObjectMap;
		GameObject* parent;
	};

	// systems
	void Renderer(olc::PixelGameEngine* aEngine)
	{
		for (auto& mySprite : spriteList)
		{
			if (mySprite->isActive)
			{
				GameObject* myParent = mySprite->parent;
				TransformSum* myTransformSum = getTransformSum(myParent);

				if (mySprite->parent->getComponentMap().count("animation"))
				{
					Animation* myAnimtation = (Animation*)mySprite->parent->getComponentMap().find("animation")->second;
					int tilesX = myAnimtation->tileSetSize[0] / myAnimtation->tileSize[0];
					int TilesY = myAnimtation->tileSetSize[1] / myAnimtation->tileSize[1];
					int tile = myAnimtation->tile;
					aEngine->DrawPartialDecal({ myTransformSum->x, myTransformSum->y }, { (float)myAnimtation->tileSize[0] * myTransformSum->scaleX,
						(float)myAnimtation->tileSize[1] * myTransformSum->scaleY }, mySprite->objectDecal, { (float)myAnimtation->tileSize[0] * (tile % tilesX), 
						(float)myAnimtation->tileSize[1] * (tile / TilesY) }, { (float)myAnimtation->tileSize[0], (float)myAnimtation->tileSize[1] });
				}
				else
				{
					aEngine->DrawDecal({ myTransformSum->x, myTransformSum->y }, mySprite->objectDecal, { myTransformSum->scaleX, myTransformSum->scaleY});
				}
			}
		}
	}

	void collision()
	{
		int collisionGridSize = 32;

		for (auto& myBoxCollider : boxColliderList)
		{
			Transform* myTransform = (Transform*)myBoxCollider->parent->getComponentMap().find("transform")->second;
			//float pos1[2] = { myBoxCollider->pos[0], myBoxCollider->pos[1] };
		}
	}

	//other functions and classes
	struct TransformSum
	{
		TransformSum(float aX, float aY, float aScaleX, float aScaleY)
		{
			x = aX;
			y = aY;
			scaleX = aScaleX;
			scaleY = aScaleY;
		}
		float x;
		float y;
		float scaleX;
		float scaleY;
	};

	TransformSum* getTransformSum(GameObject* myParent)
	{
		TransformSum* myTransformSum;

		float myX = 0.0f;
		float myY = 0.0f;
		float myScaleX = 1.0f;
		float myScaleY = 1.0f;

		while (myParent)
		{
			Transform* myTransform = (Transform*)myParent->getComponentMap().find("transform")->second;

			myX += myTransform->x;
			myY += myTransform->y;
			myScaleX *= myTransform->scaleX;
			myScaleY *= myTransform->scaleY;

			myParent = myParent->getParent();
		}

		myTransformSum = new TransformSum(myX, myY, myScaleX, myScaleY);
	}
}
