#pragma once

#include "BehaviourScript.h"
#include "Input.h"
#include "Timers.h"
#include "Cursor.h"

class CameraOrbit : public BehaviourScript {
public:
	// Update is called every frame
	void Update() {
		//std::cout << "Camera Rotation" << std::endl;

		movement();

		MouseCursor::SetVisible(!Input::GetMouseButtonDown(1));
		if (Input::GetMouseButtonDown(1))
		{
			rotation();
		}
	}

	float flightSpeed = 0.1;

	void movement()
	{
		if (Input::GetKeyDown(GLFW_KEY_W))
			transform.position += transform.getForwardVector() * Time::deltaTime;
		if (Input::GetKeyDown(GLFW_KEY_S))
			transform.position -= transform.getForwardVector() * Time::deltaTime;
		if (Input::GetKeyDown(GLFW_KEY_A))
			transform.position -= transform.getRightVector() * Time::deltaTime;
		if (Input::GetKeyDown(GLFW_KEY_D))
			transform.position += transform.getRightVector() * Time::deltaTime;
	}

	float x = 0.0;
	float y = 0.0;

	float mouseSensitivity = 0.2;

	void rotation()
	{
		x -= Input::GetAxis("Mouse X") * mouseSensitivity;
		y -= Input::GetAxis("Mouse Y") * mouseSensitivity;

		//std::cout << x << std::endl;

		bool constrainPitch = true;

		// Make sure that when pitch is out of bounds, screen doesn't get flipped
		if (y > 89.0f)
			y = 89.0f;
		if (y < -89.0f)
			y = -89.0f;

		//std::cout << y << std::endl;

#ifdef USE_GLM_QUATERNION
		transform.rotation = glm::quat(glm::vec3(y / 180 * M_PI, x / 180 * M_PI, 0));
#else
		transform.rotation = Quaternion(glm::vec3(y, x, 0));
#endif

		//std::cout << transform.rotation.axisAngles.x << ", " << transform.rotation.y << ", " << transform.rotation.z << std::endl;
	}

	CameraOrbit(Transform &newTransform) : BehaviourScript(newTransform)
	{

	}
};