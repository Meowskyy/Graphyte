#pragma once

#include "ECS.h"
#include "Physics\Physics3D.h"

namespace Graphyte {
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
		float bounciness = 0.0f;
		Vector3 momentum;
		Vector3 velocity;
		Vector3 force;

		Rigidbody() = default;

		Vector3 ComputeForces() const;
		void AddForce(const Vector3 force);

		void FixedUpdate();

		void OnCollisionEnter(const GameObject& gameObject) {
			//std::cout << "Collision with: " << gameObject->transform.name << "\n";
		}

		void OnCollisionExit()
		{

		}

		void Recalculate()
		{
			velocity = momentum * -mass;
		}

		void DrawUI();
	};
}