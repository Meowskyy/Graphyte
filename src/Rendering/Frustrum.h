#pragma once

#include "glm\gtc\matrix_transform.hpp"
#include "Math\Math.h"

class Transform;

class Frustum
{
private:
	enum Plane
	{
		PLANE_FAR,
		PLANE_NEAR,
		PLANE_RIGHT,
		PLANE_LEFT,
		PLANE_TOP,
		PLANE_BOTTOM
	};
public:
	Frustum() {}

	// model * view * projection = model space
	// view * projection = world space
	// projection = eye-space
	Frustum(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix);
	
	// TODO: Not in use currently
	Frustum(const glm::mat4& viewMatrix, const glm::mat4& projectionMatrix, const glm::mat4& modelMatrix);

	bool TestIntersection(const Transform& transform);

	Vector4 planes[6];        // plane normals point into frustum
};