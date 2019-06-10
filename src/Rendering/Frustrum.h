#pragma once

#include "glm\gtc\matrix_transform.hpp"
#include "Math\Math.h"

namespace Graphyte {
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
		Frustum(const Matrix4& viewMatrix, const Matrix4& projectionMatrix);

		// TODO: Not in use currently
		Frustum(Matrix4& viewMatrix, const Matrix4& projectionMatrix, const Matrix4& modelMatrix);

		bool TestIntersection(const Transform& transform);

		Vector4 planes[6];        // plane normals point into frustum
	};
}