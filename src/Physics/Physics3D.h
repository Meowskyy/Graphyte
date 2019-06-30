#pragma once

#include "Math\Math.h"


namespace Graphyte {
	class Camera;

	class Physics {
	private:

		static bool TRI_TRI_3D(Vector3 p1, Vector3 q1, Vector3 r1, Vector3 p2, Vector3 q2, Vector3 r2, double dp2, double dq2, double dr2);
		static bool TRI_TRI_INTER_3D(Vector3 p1, Vector3 q1, Vector3 r1, Vector3 p2, Vector3 q2, Vector3 r2, double dp2, double dq2, double dr2);
		// Three-dimensional Triangle-Triangle Overlap Test
		// additionaly computes the segment of intersection of the two triangles if it exists. 
		// coplanar returns whether the triangles are coplanar, 
		// source and target are the endpoints of the line segment of intersection 
		static bool tri_tri_intersection_test_3d(Vector3 p1, Vector3 q1, Vector3 r1, Vector3 p2, Vector3 q2, Vector3 r2, int* coplanar, Vector3 source, Vector3 target);
		static bool coplanar_tri_tri3d(Vector3 p1, Vector3  q1, Vector3  r1, Vector3  p2, Vector3  q2, Vector3  r2, Vector3  N1, Vector3  N2);
	public:
		static Vector3 RaycastMousePosition(const float distance);
		static Vector3 RaycastMouseDirection(const Camera& camera);
		static Vector3 RaycastMouseDirection();
		static bool TestRayOBBIntersection(Vector3 ray_origin, Vector3 ray_direction, Vector3 aabb_min, Vector3 aabb_max, Matrix4 ModelMatrix, float& intersection_distance);
	
		// Three-dimensional Triangle-Triangle Overlap Test
		static bool tri_tri_overlap_test_3d(Vector3 p1, Vector3 q1, Vector3 r1, Vector3 p2, Vector3 q2, Vector3 r2);
		// Two-dimensional Triangle-Triangle Overlap Test
		static bool tri_tri_overlap_test_2d(Vector2 p1, Vector2 q1, Vector2 r1, Vector2 p2, Vector2 q2, Vector2 r2);
	};
}

