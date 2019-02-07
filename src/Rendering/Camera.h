#pragma once

#include "ECS.h"
#include "glm\gtc\matrix_transform.hpp"

#include <vector>

#include "Frustrum.h"

#include <iostream>

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera : public Component
{
private:
	glm::mat4 projection;
public:
	Frustum frustrum;
	static Camera* mainCamera;

	// Camera options
	float fov = 70.0f;
	float farClipPlane = 1000.0f;
	float nearClipPlane = 0.1f;

	// Constructor
	Camera() 
	{
		// TODO: Update projection whenever any variable changes
		projection = glm::perspective(glm::radians(fov), (float)1920 / (float)1080, nearClipPlane, farClipPlane);
	}

	void Update() {
		frustrum = Frustum(GetViewMatrix(), GetProjectionMatrix());
	}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjectionMatrix();
};