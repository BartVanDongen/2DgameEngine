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
	class Transform;
	class Sprite;
	class Animation;
	class BoxCollider;
	class GameObject;

	// component list
	vector<Transform*> transformList;
	vector<Sprite*> spriteList;
	vector<Animation*> animationList;
	vector<BoxCollider*> boxColliderList;

	// components
	class Component
	{
	public:
		Component() {}

		virtual string getName()
		{
			return "component";
		}
	};

	class Transform : public Component
	{
	public:
		Transform(GameObject* aParent, int aX = 0.0f, int aY= 0.0f, float aScaleX = 1.0f, float aScaleY = 1.0f)
		{
			x = aX;
			y = aY;
			scaleX = aScaleX;
			scaleY = aScaleY;
			parent = aParent;
			transformList.push_back(this);
		}

		string getName()
		{
			return "transform";
		}

		GameObject* getParent()
		{
			return parent;
		}

		float getX()
		{
			return x;
		}

		float getY()
		{
			return y;
		}

		float getScaleX()
		{
			return scaleX;
		}

		float getScaleY()
		{
			return scaleY;
		}

	private:
		float x;
		float y;
		float scaleX;
		float scaleY;
		GameObject* parent;
	};

	class Animation : public Component
	{
	public:
		Animation(GameObject* aParent, string aTileSetName)
		{
			json myJson;
			string dir = "assets/" + aTileSetName + ".json";
			ifstream ifslevel(dir);
			myJson = json::parse(ifslevel);
			tileSetSize.push_back(myJson.find("tileSetSize").value()[0]);
			tileSetSize.push_back(myJson.find("tileSetSize").value()[1]);
			tileSize.push_back(myJson.find("tileSize").value()[0]);
			tileSize.push_back(myJson.find("tileSize").value()[1]);
			for (const auto& anim : myJson.find("Animation").value().items())
				animationMap.insert(pair<string, vector<int>>(anim.key(), anim.value()));
			parent = aParent;
			animationList.push_back(this);
		}

		string getName()
		{
			return "animation";
		}

		GameObject* getParent()
		{
			return parent;
		}

		int getTile()
		{
			return tile;
		}

		vector<int> getTileSetSize()
		{
			return tileSetSize;
		}

		vector<int> getTileSize()
		{
			return tileSize;
		}

		map<string, vector<int>> getAnimationMap()
		{
			return animationMap;
		}

		void setTile(int aTile)
		{
			tile = aTile;
		}

		void setTileSetSize(vector<int> aTileSetSize)
		{
			tileSetSize = aTileSetSize;
		}

		void setTileSize(vector<int> aTileSize)
		{
			tileSize = aTileSize;
		}

	private:
		vector<int> tileSetSize;
		vector<int> tileSize;
		int tile = 1;
		map<string, vector<int>> animationMap;
		GameObject* parent;
	};

	class Sprite : public Component
	{
	public:
		Sprite(GameObject* aParent, string aTileSetName)
		{
			string dir = "assets/" + aTileSetName + ".png";
			objectDecal = new olc::Decal(new olc::Sprite(dir));
			parent = aParent;
			spriteList.push_back(this);
		}

		string getName()
		{
			return "sprite";
		}

		GameObject* getParent()
		{
			return parent;
		}

		olc::Decal* getDecal()
		{
			return objectDecal;
		}

		bool getActive()
		{
			return isActive;
		}

		void setActive(bool aBool)
		{
			isActive = aBool;
		}

	private:
		olc::Decal* objectDecal;
		bool isActive = true;
		GameObject* parent;
	};

	class BoxCollider : public Component
	{
	public:
		BoxCollider(GameObject* aParent)
		{
			parent = aParent;
			boxColliderList.push_back(this);
		}

		string getName()
		{
			return "BoxCollider";
		}

		GameObject* getParent()
		{
			return parent;
		}

		bool getActive()
		{
			return isActive;
		}

		void setActive(bool aBool)
		{
			isActive = aBool;
		}

		BoxCollider* getCollidingWith()
		{
			return collidingWith;
		}

		void setCollidingWith(BoxCollider* aCollidingWith)
		{
			collidingWith = aCollidingWith;
		}

	private:
		bool isActive = true;
		BoxCollider* collidingWith;
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
			string name = aComponent->getName();
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
			if (mySprite->getActive())
			{
				float myX = 0.0f;
				float myY = 0.0f;
				float myScaleX = 1.0f;
				float myScaleY = 1.0f;

				GameObject* myParent = mySprite->getParent();
				while(myParent)
				{
					Transform* myTransform = (Transform*)myParent->getComponentMap().find("transform")->second;
					
					myX += myTransform->getX();
					myY += myTransform->getY();
					myScaleX *= myTransform->getScaleX();
					myScaleY *= myTransform->getScaleY();

					myParent = myParent->getParent();
				}

				if (mySprite->getParent()->getComponentMap().count("animation"))
				{
					Animation* myAnimtation = (Animation*)mySprite->getParent()->getComponentMap().find("animation")->second;
					int tilesX = myAnimtation->getTileSetSize()[0] / myAnimtation->getTileSize()[0];
					int TilesY = myAnimtation->getTileSetSize()[1] / myAnimtation->getTileSize()[1];
					int tile = myAnimtation->getTile();
					aEngine->DrawPartialDecal({ myX, myY }, { (float)myAnimtation->getTileSize()[0] * myScaleX,(float)myAnimtation->getTileSize()[1] * myScaleY}, 
						mySprite->getDecal(), { (float)myAnimtation->getTileSize()[0] * (tile % tilesX), (float)myAnimtation->getTileSize()[1] * (tile / TilesY) },
						{ (float)myAnimtation->getTileSize()[0], (float)myAnimtation->getTileSize()[1]});
				}
				else
				{
					aEngine->DrawDecal({ myX, myY }, mySprite->getDecal(), { myScaleX, myScaleY });
				}
			}
		}
	}

	void collision2()
	{

	}

	void collision()
	{
		// idea : put all boxCollider dimentions in vector and calculate them all at the same time

		for (auto& myBoxCollider1 : boxColliderList)
		{
			float boxCollider1X = 0.0f;
			float boxCollider1Y = 0.0f;

			float myScale1X = 1.0f;
			float myScale1Y = 1.0f;

			GameObject* myParent1 = myBoxCollider1->getParent();
			Animation* myAnimation1 = (Animation*)myParent1->getComponentMap().find("sprite")->second;
			while (myParent1)
			{

				Transform* myTransform = (Transform*)myParent1->getComponentMap().find("transform")->second;
				boxCollider1X += myTransform->getX();
				boxCollider1Y += myTransform->getY();
				myScale1X *= myTransform->getScaleX();
				myScale1Y *= myTransform->getScaleY();
				myParent1 = myParent1->getParent();
			}

			float myColliderMid1X = boxCollider1X + (myAnimation1->getTileSize()[0] * myScale1X / 2);
			float myColliderMid1Y = boxCollider1Y + (myAnimation1->getTileSize()[1] * myScale1Y / 2);

			for (auto& myBoxCollider2 : boxColliderList)
			{
				if (myBoxCollider1 != myBoxCollider2)
				{
					float boxCollider2X = 0.0f;
					float boxCollider2Y = 0.0f;

					float myScale2X = 1.0f;
					float myScale2Y = 1.0f;

					GameObject* myParent2 = myBoxCollider2->getParent();
					Animation* myAnimation2 = (Animation*)myParent2->getComponentMap().find("sprite")->second;
					while (myParent2)
					{
						Transform* myTransform = (Transform*)myParent2->getComponentMap().find("transform")->second;
						boxCollider2X += myTransform->getX();
						boxCollider2Y += myTransform->getY();
						myScale2X *= myTransform->getScaleX();
						myScale2Y *= myTransform->getScaleY();
						myParent2 = myParent2->getParent();
					}

					float myMaxCollisionDistanceX = (myAnimation1->getTileSize()[0] * myScale1X + myAnimation2->getTileSize()[0] * myScale2X) / 2;
					float myMaxCollisionDistanceY = (myAnimation1->getTileSize()[1] * myScale1Y + myAnimation2->getTileSize()[1] * myScale2Y) / 2;

					float myColliderMid2X = boxCollider2X + (myAnimation2->getTileSize()[0] * myScale2X / 2);
					float myColliderMid2Y = boxCollider2Y + (myAnimation2->getTileSize()[1] * myScale2Y / 2);

					if (abs(myColliderMid1X - myColliderMid2X) <= myMaxCollisionDistanceX && abs(myColliderMid1Y - myColliderMid2Y) <= myMaxCollisionDistanceY)
					{
						myBoxCollider1->setActive(true);
					}
					else
					{
						myBoxCollider1->setActive(false);
					}
				}
			}
		}
	}
}