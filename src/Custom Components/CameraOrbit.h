#pragma once

#include "ECS.h"

#include "Input\Input.h"
#include "Time\Timers.h"
#include "Cursor\Cursor.h"

namespace Graphyte {
	class CameraOrbit : public Component {
	public:
		float x = 0.0;
		float y = 0.0;

		float flightSpeedMul = 1;

		void OnComponentAdded()
		{
			x = transform->rotation.y * PI;
			y = transform->rotation.x * PI;
		}

		void Update()
		{
			if (Input::GetKeyDown(GLFW_KEY_C)) {
				ExtraRenderer::DrawLine(transform->position + transform->GetForwardVector(), Physics::RaycastMousePosition(100));
			}

			if (Input::GetKeyDown(GLFW_KEY_K))
				Scene::DestroyLast();
		}

		// Update is called every frame
		void LateUpdate() {
			movement();

			//MouseCursor::SetVisible(!Input::GetMouseButtonDown(1));
			if (Input::GetMouseButtonDown(1))
			{
				rotation();
			}
		}

		float flightSpeed = 3;

		void movement()
		{
			if (Input::GetKeyDown(GLFW_KEY_W))
				transform->position += transform->GetForwardVector() * Time::deltaTime * flightSpeed * flightSpeedMul;
			if (Input::GetKeyDown(GLFW_KEY_S))
				transform->position -= transform->GetForwardVector() * Time::deltaTime * flightSpeed * flightSpeedMul;
			if (Input::GetKeyDown(GLFW_KEY_A))
				transform->position -= transform->GetRightVector() * Time::deltaTime * flightSpeed * flightSpeedMul;
			if (Input::GetKeyDown(GLFW_KEY_D))
				transform->position += transform->GetRightVector() * Time::deltaTime * flightSpeed * flightSpeedMul;

			if (Input::GetKeyDown(GLFW_KEY_W) || Input::GetKeyDown(GLFW_KEY_S) || Input::GetKeyDown(GLFW_KEY_D) || Input::GetKeyDown(GLFW_KEY_A)) {
				flightSpeedMul += 1.5f * Time::deltaTime;
			}
			else {
				flightSpeedMul = 1;
			}
		}

		float mouseSensitivity = 0.005f;

		void rotation()
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

		/*
		void DrawUI() {
			ImGui::DragFloat("Rotation X", &x);
			ImGui::DragFloat("Rotation Y", &y);
		}
		*/
	};
}