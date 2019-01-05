#pragma once

#include "Scripting\BehaviourScript.h"
#include "glm\gtc\matrix_transform.hpp"

#include <vector>

// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera : public BehaviourScript
{
public:
	static Camera* mainCamera;

	// Camera options
	float fov = 60;
	float renderDistance = 100;

	// Constructor
	Camera() {}

	// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
	glm::mat4 GetViewMatrix();
};