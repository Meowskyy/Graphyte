#pragma once

#include "ECS.h"
#include "glm\gtc\matrix_transform.hpp"

#include <vector>

#include "Frustrum.h"

#include <iostream>
namespace Graphyte {
	class Camera : public Component
	{
	public:
		static Camera* mainCamera;

		Frustum frustrum;

		// Camera options
		float fov = 70.0f;
		float farClipPlane = 1000.0f;
		float nearClipPlane = 0.1f;

		// Constructor
		Camera() {}

		void Update()
		{
			frustrum = Frustum(GetViewMatrix(), GetProjectionMatrix());
		}

		// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
		Matrix4 GetViewMatrix();
		Matrix4 GetProjectionMatrix();

		void DrawUI();
	};
}