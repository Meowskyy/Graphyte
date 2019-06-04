#pragma once

#include "ECS.h"

#include "Input\Input.h"
#include "Time\Timers.h"
#include "Cursor\Cursor.h"

namespace Graphyte {
	class EditorCamera : public Component {
	public:
		float mouseSensitivity = 0.005f;

		float x = 0.0;
		float y = 0.0;

		float flightSpeedMul = 1;

		void OnComponentAdded()
		{
			x = 0;
			y = PI;
		}

		// LateUpdate is called at the end of every frame
		void LateUpdate() 
		{
			Movement();

			//MouseCursor::SetVisible(!Input::GetMouseButtonDown(1));
			if (Input::GetMouseButtonDown(1))
			{
				Rotation();
			}
		}

		float flightSpeed = 3;

		void Movement()
		{
			if (Input::GetKeyDown(GLFW_KEY_W))
				transform->position += transform->getForwardVector() * Time::deltaTime * flightSpeed * flightSpeedMul;
			if (Input::GetKeyDown(GLFW_KEY_S))
				transform->position -= transform->getForwardVector() * Time::deltaTime * flightSpeed * flightSpeedMul;
			if (Input::GetKeyDown(GLFW_KEY_A))
				transform->position -= transform->getRightVector() * Time::deltaTime * flightSpeed * flightSpeedMul;
			if (Input::GetKeyDown(GLFW_KEY_D))
				transform->position += transform->getRightVector() * Time::deltaTime * flightSpeed * flightSpeedMul;

			if (Input::GetKeyDown(GLFW_KEY_W) || Input::GetKeyDown(GLFW_KEY_S) || Input::GetKeyDown(GLFW_KEY_D) || Input::GetKeyDown(GLFW_KEY_A)) {
				flightSpeedMul += 1.5f * Time::deltaTime;
			}
			else {
				flightSpeedMul = 1;
			}
		}

		void Rotation()
		{
			x -= Input::GetAxis("Mouse X") * mouseSensitivity;
			y += Input::GetAxis("Mouse Y") * mouseSensitivity;

			bool constrainPitch = true;

			// Make sure that when pitch is out of bounds, screen doesn't get flipped
			if (y > 89.0f)
				y = 89.0f;
			if (y < -89.0f)
				y = -89.0f;

			transform->rotation = Quaternion(Vector3(y, x, 0));
		}
	};
}