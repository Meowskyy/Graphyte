#pragma once

#include "ECS.h"
#include "Physics\Physics3D.h"

class BoxShape {
public:
	float width = 1;
	float height = 1;
	float mass = 10;
	float momentOfInertia;
};

class Rigidbody : public Component {
private:

public:
	float mass = 1000;
	Vector3 velocity;

	Rigidbody() {}

	Vector3 ComputeForces();

	void FixedUpdate();

	void OnCollisionEnter(GameObject& gameObject) {
		//std::cout << "Collision with: " << gameObject->transform.name << "\n";
	}

	void OnCollisionExit() 
	{

	}

	void DrawUI();
};