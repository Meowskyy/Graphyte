#pragma once

#include "Scripting\BehaviourScript.h"

#include <vector>
#include <map>

class GameObject {
private:
	std::vector<BehaviourScript*> behaviour;
	std::map<std::string, int> behaviourDict;

	int behaviourCount = 0;
	int childCount = 0;
public:
	// Is the GameObject active?
	bool enabled = true;

	bool hasCollision = true;

	Transform transform = Transform();
	std::vector<GameObject*> children;
	std::vector<GameObject*> collisionList;

	GameObject() {}
	~GameObject() {}

	void Update();
	void FixedUpdate();
	void OnSceneLoad();

	void OnRigidbodyCollisionEnter();

	void DrawBehaviours();
	void DrawChildren();

	void AddChild(GameObject* object);
	void RemoveChild(int childIndex);

	// Adds a behaviour of type T to the object
	BehaviourScript* AddBehaviour(BehaviourScript* script);
	// Get the first behaviour that has the same name
	BehaviourScript* GetBehaviour(std::string name);
	// Removes the first behaviour with the same name
	void RemoveBehaviour(std::string name);
};