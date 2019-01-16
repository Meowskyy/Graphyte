#pragma once

#include "ECS.h"

#include "Input\Input.h"
#include "Time\Timers.h"
#include "Cursor\Cursor.h"

const float PI = 3.141592653589793;

class CameraOrbit : public Component {
public:
	float x = 0.0;
	float y = 0.0;

	void OnComponentAdded() 
	{
		x = 0;
		y = PI;
	}

	// Update is called every frame
	void Update() {
		movement();

		//MouseCursor::SetVisible(!Input::GetMouseButtonDown(1));
		if (Input::GetMouseButtonDown(1))
		{
			rotation();
		}
	}

	float flightSpeed = 250;

	void movement()
	{
		if (Input::GetKeyDown(GLFW_KEY_W))
			transform->position += transform->getForwardVector() * Time::deltaTime * flightSpeed;
		if (Input::GetKeyDown(GLFW_KEY_S))
			transform->position -= transform->getForwardVector() * Time::deltaTime * flightSpeed;
		if (Input::GetKeyDown(GLFW_KEY_A))
			transform->position -= transform->getRightVector() * Time::deltaTime * flightSpeed;
		if (Input::GetKeyDown(GLFW_KEY_D))
			transform->position += transform->getRightVector() * Time::deltaTime * flightSpeed;
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