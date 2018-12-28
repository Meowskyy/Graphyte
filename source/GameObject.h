#pragma once

#include <vector>

#include "BehaviourScript.h"

class GameObject {
public:
	Transform transform = Transform();
	std::vector<BehaviourScript*> behaviour;

	GameObject();
	~GameObject();

	void AddBehaviour(BehaviourScript* script);

	template <class T>
	T GetBehaviour();

	template <class T>
	T GetBehaviourByUUID(int UUID);
};