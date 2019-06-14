#pragma once

#include "Math\Math.h"


namespace Graphyte {
	class Camera;

	class Physics {
	public:
		static Vector3 RaycastMousePosition(const float distance);
		static Vector3 RaycastMouseDirection(const Camera& camera);
		static Vector3 RaycastMouseDirection();
		static bool TestRayOBBIntersection(Vector3 ray_origin, Vector3 ray_direction, Vector3 aabb_min, Vector3 aabb_max, Matrix4 ModelMatrix, float& intersection_distance);
	};
}

