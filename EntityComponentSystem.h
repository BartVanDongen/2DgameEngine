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
	enum class componentKey : int { component, transform, sprite, animation, boxCollider};

	struct Component;
	struct Transform;
	struct Sprite;
	struct Animation;
	struct BoxCollider;
	struct TransformSum;

	vector<Transform*> transformList;
	vector<Sprite*> spriteList;
	vector<Animation*> animationList;
	vector<BoxCollider*> boxColliderList;

	//----------------------------------//
	//		Entity definition			//
	//----------------------------------//

	class GameObject
	{
	public:
		GameObject(GameObject* aParent = NULL);

		void addComponent(Component* aComponent);

		void addChild(GameObject aGameObject);

		GameObject* getParent();

		map<componentKey, Component*> getComponentMap();

	private:
		bool hasParent = false;
		map<componentKey, Component*> componentMap;
		vector<GameObject> gameObjectMap;
		GameObject* parent;
	};

	//----------------------------------//
	//		Component definition		//
	//----------------------------------//

	struct Component
	{
		Component() {}

		virtual componentKey getName();
	};

	struct Transform : public Component
	{
		Transform(GameObject* aParent, float aX = 0.0f, float aY = 0.0f, float aScaleX = 1.0f, float aScaleY = 1.0f);

		componentKey getName();

		float x;
		float y;
		float scaleX;
		float scaleY;
		GameObject* parent;
	};

	struct Sprite : public Component
	{
		Sprite(GameObject* aParent, string aTileSetName);

		componentKey getName();

		olc::Decal* objectDecal;
		bool isActive = true;
		GameObject* parent;
	};

	struct Animation : public Component
	{
		Animation(GameObject* aParent, string aTileSetName);

		componentKey getName();

		int tileW;
		int tileH;
		int tileSetW;
		int tileSetH;
		int tile = 1;
		map<string, vector<int>> animationMap;
		GameObject* parent;
	};

	struct BoxCollider : public Component
	{
		// sprite size
		BoxCollider(GameObject* aParent);

		// custom size
		BoxCollider(GameObject* aParent, float aX, float aY, int aW, int aH);

		componentKey getName();

		bool isActive = true;
		float x;
		float y;
		int w;
		int h;
		vector<BoxCollider*> collidingWith;
		GameObject* parent;
	};

	//----------------------------------//
	//		System definition			//
	//----------------------------------//

	struct CollisionPos
	{
		CollisionPos(float aPos1[2], float aPos2[2], BoxCollider* aBoxCollider);

		void setActive(BoxCollider* aBoxCollider);

		float pos1[2];
		float pos2[2];
		BoxCollider* boxCollider;
	};

	struct Block
	{
		Block(CollisionPos* aBoxCollider);

		void addCollider(CollisionPos* aBoxCollider);

		vector<CollisionPos*> collidersList;
	};

	struct TransformSum
	{
		TransformSum(float aX, float aY, float aScaleX, float aScaleY);
		float x;
		float y;
		float scaleX;
		float scaleY;
	};

	TransformSum* getTransformSum(GameObject* myParent);

	void Renderer(olc::PixelGameEngine* aEngine);

	void collision();

	//----------------------------------//
	//		Entity implimentation		//
	//----------------------------------//

	GameObject::GameObject(GameObject* aParent)
	{
		parent = aParent;
	}

	void GameObject::addComponent(Component* aComponent)
	{
		componentKey name = aComponent->getName();
		componentMap.insert(pair<componentKey, Component*>(name, aComponent));
	}

	void GameObject::addChild(GameObject aGameObject)
	{
		aGameObject.hasParent = true;
		gameObjectMap.push_back(aGameObject);
	}

	GameObject* GameObject::getParent()
	{
		return parent;
	}

	map<componentKey, Component*> GameObject::getComponentMap()
	{
		return componentMap;
	}

	//----------------------------------//
	//		Component implimentetion	//
	//----------------------------------//

	componentKey Component::getName()
	{
		return componentKey::component;
	}

	Transform::Transform(GameObject* aParent, float aX, float aY, float aScaleX, float aScaleY)
	{
		x = aX;
		y = aY;
		scaleX = aScaleX;
		scaleY = aScaleY;
		parent = aParent;
		transformList.push_back(this);
	}

	componentKey Transform::getName()
	{
		return componentKey::transform;
	}

	Sprite::Sprite(GameObject* aParent, string aTileSetName)
	{
		objectDecal = new olc::Decal(new olc::Sprite("assets/" + aTileSetName + ".png"));
		parent = aParent;
		spriteList.push_back(this);
	}

	componentKey Sprite::getName()
	{
		return componentKey::sprite;
	}

	Animation::Animation(GameObject* aParent, string aTileSetName)
	{
		ifstream ifslevel("assets/" + aTileSetName + ".json");
		json myJson = json::parse(ifslevel);
		tileSetW = myJson.find("tileSetSize").value()[0];
		tileSetH = myJson.find("tileSetSize").value()[1];
		tileW = myJson.find("tileSize").value()[0];
		tileH = myJson.find("tileSize").value()[1];
		animationMap = myJson["Animation"].get<map<string, vector<int>>>();
		parent = aParent;
		animationList.push_back(this);
	}

	componentKey Animation::getName()
	{
		return componentKey::animation;
	}

	// sprite size
	BoxCollider::BoxCollider(GameObject* aParent)
	{
		parent = aParent;
		Animation* myAnimation = (Animation*)aParent->getComponentMap().find(componentKey::sprite)->second;
		w = myAnimation->tileW;
		h = myAnimation->tileH;
		boxColliderList.push_back(this);
	}

	// custom size
	BoxCollider::BoxCollider(GameObject* aParent, float aX, float aY, int aW, int aH)
	{
		parent = aParent;
		x = aX;
		y = aY;
		w = aW;
		h = aH;
		boxColliderList.push_back(this);
	}

	componentKey BoxCollider::getName()
	{
		return componentKey::boxCollider;
	}

	//----------------------------------//
	//		System implimentation		//
	//----------------------------------//

	CollisionPos::CollisionPos(float aPos1[2], float aPos2[2], BoxCollider* aBoxCollider)
	{
		pos1[0] = aPos1[0];
		pos1[1] = aPos1[1];
		pos2[0] = aPos2[0];
		pos2[1] = aPos2[1];
		boxCollider = aBoxCollider;
	}

	void CollisionPos::setActive(BoxCollider* aBoxCollider)
	{
		boxCollider->collidingWith.push_back(aBoxCollider);
	}

	Block::Block(CollisionPos* aBoxCollider)
	{
		collidersList.push_back(aBoxCollider);
	}

	void Block::addCollider(CollisionPos* aBoxCollider)
	{
		collidersList.push_back(aBoxCollider);
	}

	TransformSum::TransformSum(float aX, float aY, float aScaleX, float aScaleY)
	{
		x = aX;
		y = aY;
		scaleX = aScaleX;
		scaleY = aScaleY;
	}

	TransformSum* getTransformSum(GameObject* myParent)
	{
		TransformSum* myTransformSum;

		float myX = 0.0f;
		float myY = 0.0f;
		float myScaleX = 1.0f;
		float myScaleY = 1.0f;

		while (myParent)
		{
			Transform* myTransform = (Transform*)myParent->getComponentMap().find(componentKey::transform)->second;

			myX += myTransform->x;
			myY += myTransform->y;
			myScaleX *= myTransform->scaleX;
			myScaleY *= myTransform->scaleY;

			myParent = myParent->getParent();
		}

		myTransformSum = new TransformSum(myX, myY, myScaleX, myScaleY);

		return myTransformSum;
	}

	void Renderer(olc::PixelGameEngine* aEngine)
	{
		for (auto& mySprite : spriteList)
		{
			if (mySprite->isActive)
			{
				GameObject* myParent = mySprite->parent;
				TransformSum* myTransformSum = getTransformSum(myParent);

				if (mySprite->parent->getComponentMap().count(componentKey::animation))
				{
					Animation* myAnimtation = (Animation*)mySprite->parent->getComponentMap().find(componentKey::animation)->second;
					int tilesX = myAnimtation->tileSetW / myAnimtation->tileW;
					int TilesY = myAnimtation->tileSetH / myAnimtation->tileH;
					int tile = myAnimtation->tile;
					aEngine->DrawPartialDecal({ myTransformSum->x, myTransformSum->y }, { (float)myAnimtation->tileW * myTransformSum->scaleX,
						(float)myAnimtation->tileH * myTransformSum->scaleY }, mySprite->objectDecal, { (float)myAnimtation->tileW * (tile % tilesX),
						(float)myAnimtation->tileH * (tile / TilesY) }, { (float)myAnimtation->tileW, (float)myAnimtation->tileH });
				}
				else
				{
					aEngine->DrawDecal({ myTransformSum->x, myTransformSum->y }, mySprite->objectDecal, { myTransformSum->scaleX, myTransformSum->scaleY });
				}
			}
		}
	}

	void collision()
	{
		int collisionGridSize = 32;

		map<pair<int, int>, Block*> blockGridMap;

		//broad phase
		for (auto& myBoxCollider : boxColliderList)
		{
			if (!myBoxCollider->isActive)
				break;

			CollisionPos* myCollisionPos;

			float pos1[2] = {
				myBoxCollider->x + getTransformSum(myBoxCollider->parent)->x,
				myBoxCollider->y + getTransformSum(myBoxCollider->parent)->y
			};

			float pos2[2] =
			{
				pos1[0] + myBoxCollider->w * getTransformSum(myBoxCollider->parent)->scaleX,
				pos1[1] + myBoxCollider->h * getTransformSum(myBoxCollider->parent)->scaleY
			};

			myCollisionPos = new CollisionPos(pos1, pos2, myBoxCollider);

			int gridMinX = (int)ceil(pos1[0] / collisionGridSize);
			int gridMinY = (int)ceil(pos1[1] / collisionGridSize);
			int gridMaxX = (int)ceil(pos2[0] / collisionGridSize);
			int gridMaxY = (int)ceil(pos2[1] / collisionGridSize);

			for (int i = gridMinX; gridMinX <= gridMaxX; i++)
			{
				for (int j = gridMinY; gridMinY <= gridMaxY; j++)
				{
					if (blockGridMap.count(make_pair(i, j)))
						blockGridMap.find(make_pair(i, j))->second->addCollider(myCollisionPos);
					else
					{
						Block* myBlock = new Block(myCollisionPos);
						blockGridMap.insert(pair<pair<int, int>, Block*>(make_pair(i, j), myBlock));
					}
				}
			}
		}

		// remove blocks with identical colliderlists or lists that are one of size
		for (auto& blockGrid : blockGridMap)
		{

		}


		//narrow phase

	}
}