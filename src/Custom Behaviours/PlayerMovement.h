#pragma once

#include "ECS.h"
#include "Input\Input.h"
#include "Time\Timers.h"

namespace Graphyte {
	class Rigidbody;

	class PlayerMovement : public Component {
	private:

	public:
		float movementSpeed = 2.0f;

		Rigidbody* rb;

		void OnComponentAdded()
		{
			rb = &gameObject->GetComponent<Rigidbody>();
		}

		void Update()
		{
			movement();
		}

		void movement();
	};
}