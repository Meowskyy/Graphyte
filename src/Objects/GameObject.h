#pragma once

#include <vector>

#include "Scripting\BehaviourScript.h"

class GameObject {
public:
	bool enabled = true;

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