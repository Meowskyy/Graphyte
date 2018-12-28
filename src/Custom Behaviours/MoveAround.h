#pragma once

#include "BehaviourScript.h"
#include <math.h>

#include "Timers.h"

class MoveAround : public BehaviourScript {
public:
	int i = 0;
	float amount = 2;
	float speed = 0.01;

	float x = 0;
	float rotSpeed = 0.001;
	
	void Update() {
		transform.position.y = sin(i * speed) * amount;
		//std::cout << transform.position.y << std::endl;

#ifdef USE_GLM_QUATERNION
		transform.rotation = glm::quat(glm::vec3(0, i * rotSpeed, 0));
#else
		//transform.rotation = Quaternion(glm::vec3(0, i * rotSpeed, 0));
#endif

		i++;
	}

	MoveAround(Transform &newTransform) : BehaviourScript(newTransform)
	{

	}
};