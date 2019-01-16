#pragma once

#include "ECS.h"

#include <math.h>

#include "Time\Timers.h"

class MoveAround : public Component {
public:
	int i = 0;
	float amount = 2;
	float speed = 0.01f;

	float x = 0;
	float rotSpeed = 0.001f;
	
	void Update() {
		transform->position.y = sin(i * speed) * amount;

		transform->rotation = Quaternion(Vector3(0, i * rotSpeed, 0));

		i++;
	}
};