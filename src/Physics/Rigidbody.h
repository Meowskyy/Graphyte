#pragma once

#include "Scripting\BehaviourScript.h"
#include "Physics\Physics3D.h"
#include "Objects\GameObject.h"

class BoxShape {
public:
	float width = 1;
	float height = 1;
	float mass = 10;
	float momentOfInertia;
};

class Rigidbody : public BehaviourScript {
private:

public:
	float mass = 1000;
	Vector3 velocity;

	Rigidbody() {}

	Vector3 ComputeForces();

	void FixedUpdate();

	void OnCollisionEnter(GameObject* gameObject) {
		std::cout << "Collision with: " << gameObject->transform.name << "\n";
	}

	void OnCollisionExit() 
	{

	}

	void DrawUI()
	{
		ImGui::DragFloat("Mass", &mass);

		ImGui::DragFloat3("Velocity", (float*)&velocity);

		for (int i = 0; i < gameObject->collisionList.size(); i++) {
			ImGui::Text("Colliding Object: ");
			ImGui::SameLine();
			ImGui::Text(gameObject->collisionList[i]->transform.name.c_str());
		}
	}
};