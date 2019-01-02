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
	std::vector<std::string> collidingObjects;
public:
	float mass = 1000;
	Vector3 velocity;

	Rigidbody() {}

	Vector3 ComputeForces();

	void FixedUpdate();

	bool TestAABBOverlap(const Transform& otherTransform)
	{
		// TODO: Working out how to position the bounding boxes better
		// Works but not perfectly since the position is not tied to the mesh
		// The position is always 0, 0, 0 on meshes since they dont have a base position
		Vector3 aMin = transform->position + transform->boundingBox.min;
		Vector3 aMax = transform->position + transform->boundingBox.max;

		Vector3 bMin = otherTransform.position + otherTransform.boundingBox.min;
		Vector3 bMax = otherTransform.position + otherTransform.boundingBox.max;

		float d1x = bMin.x - aMax.x;
		float d1y = bMin.y - aMax.y;
		float d1z = bMin.z - aMax.z;

		float d2x = aMin.x - bMax.x;
		float d2y = aMin.y - bMax.y;
		float d2z = aMin.z - bMax.z;

		if (d1x > 0.0f || d1y > 0.0f || d1z > 0.0f)
			return false;

		if (d2x > 0.0f || d2y > 0.0f || d2z > 0.0f)
			return false;

		gameObject->OnRigidbodyCollisionEnter();
		collidingObjects.push_back(otherTransform.name);
		return true;
	}

	void DrawUI()
	{
		ImGui::DragFloat("Mass: ", &mass);

		for (int i = 0; i < collidingObjects.size(); i++) {
			ImGui::Text("Colliding Object: ");
			ImGui::SameLine();
			ImGui::Text(collidingObjects[i].c_str());
		}
	}
};