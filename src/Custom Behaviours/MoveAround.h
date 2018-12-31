#pragma once

#include "Scripting\BehaviourScript.h"
#include <math.h>

#include "Time\Timers.h"

class MoveAround : public BehaviourScript {
public:
	int i = 0;
	float amount = 2;
	float speed = 0.01;

	float x = 0;
	float rotSpeed = 0.05f;
	
	void Update() {
		transform.position.y = sin(i * speed * Time::deltaTime) * amount;
		transform.rotation = glm::quat(glm::vec3(0, i * rotSpeed * Time::deltaTime, 0));

		i++;
	}

	MoveAround(Transform &newTransform) : BehaviourScript(newTransform)
	{

	}
};