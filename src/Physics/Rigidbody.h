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
		bool isGrounded = false;
	public:
		float mass = 1000;
		float bounciness = 0.0f;
		Vector3 momentum;
		Vector3 velocity;
		Vector3 force;
		bool useGravity = true;

		Rigidbody() = default;

		Vector3 ComputeForces() const;
		void AddForce(const Vector3 force);

		void FixedUpdate();

		void OnCollisionEnter(Collider& collider) {
			isGrounded = true;
			velocity.y = 0;
		}

		void OnCollisionStay(Collider& collider) {
			//isGrounded = true;
			//velocity.y = 0;
		}

		void OnCollisionExit(Collider& collider) {
			isGrounded = false;
		}

		void Recalculate()
		{
			velocity = momentum * -mass;
		}

		void DrawUI();
	};
}