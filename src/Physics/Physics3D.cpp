#include "Physics3D.h"

#include "Input\Input.h"

#include "Rendering\Screen.h"
#include "Rendering\Camera.h"

using namespace Graphyte;

// https://github.com/capnramses/antons_opengl_tutorials_book/blob/master/07_ray_picking/main.cpp
/* takes mouse position on screen and return ray in world coords */
Vector3 Physics::RaycastMousePosition(const float distance) {
	Vector2 mousePos = Input::GetMousePosition();

	// screen space (viewport coordinates)
	float x = (2.0f * mousePos.x) / Screen::width - 1.0f;
	float y = 1.0f - (2.0f * -mousePos.y) / Screen::height;
	float z = 1.0f;
	// normalised device space
	Vector3 ray_nds = Vector3(x, y, z);
	// clip space
	Vector4 ray_clip = Vector4(ray_nds.x, ray_nds.y, -1.0, 1.0);
	// eye space
	Matrix4 projection = Camera::mainCamera->GetProjectionMatrix();
	Vector4 ray_eye = glm::inverse(projection) * ray_clip;
	ray_eye = Vector4(ray_eye.x, ray_eye.y, -1.0, 0.0);
	// world space
	Matrix4 view = Camera::mainCamera->GetViewMatrix();
	Vector3 ray_wor = Vector3(glm::inverse(view) * ray_eye);
	// don't forget to normalise the vector at some point
	ray_wor = glm::normalize(ray_wor);
	return ray_wor * distance + Camera::mainCamera->transform->position + Camera::mainCamera->transform->GetForwardVector();
}

Vector3 Physics::RaycastMouseDirection(const Camera& camera) {
	Vector2 mousePos = Input::GetMousePosition();

	// screen space (viewport coordinates)
	float x = (2.0f * mousePos.x) / Screen::width - 1.0f;
	float y = 1.0f - (2.0f * -mousePos.y) / Screen::height;
	float z = 1.0f;
	// normalised device space
	Vector3 ray_nds = Vector3(x, y, z);
	// clip space
	Vector4 ray_clip = Vector4(ray_nds.x, ray_nds.y, -1.0, 1.0);
	// eye space
	Matrix4 projection = camera.GetProjectionMatrix();
	Vector4 ray_eye = glm::inverse(projection) * ray_clip;
	ray_eye = Vector4(ray_eye.x, ray_eye.y, -1.0, 0.0);
	// world space
	Matrix4 view = camera.GetViewMatrix();
	Vector3 ray_wor = Vector3(glm::inverse(view) * ray_eye);
	// don't forget to normalise the vector at some point
	ray_wor = glm::normalize(ray_wor);
	return ray_wor;
}

Vector3 Physics::RaycastMouseDirection() {
	Vector2 mousePos = Input::GetMousePosition();

	// screen space (viewport coordinates)
	float x = (2.0f * mousePos.x) / Screen::width - 1.0f;
	float y = 1.0f - (2.0f * -mousePos.y) / Screen::height;
	float z = 1.0f;
	// normalised device space
	Vector3 ray_nds = Vector3(x, y, z);
	// clip space
	Vector4 ray_clip = Vector4(ray_nds.x, ray_nds.y, -1.0, 1.0);
	// eye space
	Matrix4 projection = Camera::mainCamera->GetProjectionMatrix();
	Vector4 ray_eye = glm::inverse(projection) * ray_clip;
	ray_eye = Vector4(ray_eye.x, ray_eye.y, -1.0, 0.0);
	// world space
	Matrix4 view = Camera::mainCamera->GetViewMatrix();
	Vector3 ray_wor = Vector3(glm::inverse(view) * ray_eye);
	// don't forget to normalise the vector at some point
	ray_wor = glm::normalize(ray_wor);
	return ray_wor;
}

bool Physics::TestRayOBBIntersection(Vector3 ray_origin, Vector3 ray_direction, Vector3 aabb_min, Vector3 aabb_max, Matrix4 ModelMatrix, float& intersection_distance)
{
	// Intersection method from Real-Time Rendering and Essential Mathematics for Games
	float tMin = 0.0f;
	float tMax = 100000.0f;

	Vector3 OBBposition_worldspace(ModelMatrix[3].x, ModelMatrix[3].y, ModelMatrix[3].z);

	Vector3 delta = OBBposition_worldspace - ray_origin;

	// Test intersection with the 2 planes perpendicular to the OBB's X axis
	{
		Vector3 xaxis(ModelMatrix[0].x, ModelMatrix[0].y, ModelMatrix[0].z);
		float e = glm::dot(xaxis, delta);
		float f = glm::dot(ray_direction, xaxis);

		if (fabs(f) > 0.001f) { // Standard case

			float t1 = (e + aabb_min.x) / f; // Intersection with the "left" plane
			float t2 = (e + aabb_max.x) / f; // Intersection with the "right" plane
			// t1 and t2 now contain distances betwen ray origin and ray-plane intersections

			// We want t1 to represent the nearest intersection, 
			// so if it's not the case, invert t1 and t2
			if (t1 > t2) {
				float w = t1; t1 = t2; t2 = w; // swap t1 and t2
			}

			// tMax is the nearest "far" intersection (amongst the X,Y and Z planes pairs)
			if (t2 < tMax)
				tMax = t2;
			// tMin is the farthest "near" intersection (amongst the X,Y and Z planes pairs)
			if (t1 > tMin)
				tMin = t1;

			// And here's the trick :
			// If "far" is closer than "near", then there is NO intersection.
			// See the images in the tutorials for the visual explanation.
			if (tMax < tMin)
				return false;

		}
		else { // Rare case : the ray is almost parallel to the planes, so they don't have any "intersection"
			if (-e + aabb_min.x > 0.0f || -e + aabb_max.x < 0.0f)
				return false;
		}
	}


	// Test intersection with the 2 planes perpendicular to the OBB's Y axis
	// Exactly the same thing than above.
	{
		Vector3 yaxis(ModelMatrix[1].x, ModelMatrix[1].y, ModelMatrix[1].z);
		float e = glm::dot(yaxis, delta);
		float f = glm::dot(ray_direction, yaxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + aabb_min.y) / f;
			float t2 = (e + aabb_max.y) / f;

			if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;

		}
		else {
			if (-e + aabb_min.y > 0.0f || -e + aabb_max.y < 0.0f)
				return false;
		}
	}

	// Test intersection with the 2 planes perpendicular to the OBB's Z axis
	// Exactly the same thing than above.
	{
		Vector3 zaxis(ModelMatrix[2].x, ModelMatrix[2].y, ModelMatrix[2].z);
		float e = glm::dot(zaxis, delta);
		float f = glm::dot(ray_direction, zaxis);

		if (fabs(f) > 0.001f) {

			float t1 = (e + aabb_min.z) / f;
			float t2 = (e + aabb_max.z) / f;

			if (t1 > t2) { float w = t1; t1 = t2; t2 = w; }

			if (t2 < tMax)
				tMax = t2;
			if (t1 > tMin)
				tMin = t1;
			if (tMin > tMax)
				return false;

		}
		else {
			if (-e + aabb_min.z > 0.0f || -e + aabb_max.z < 0.0f)
				return false;
		}
	}

	intersection_distance = tMin;
	return true;

}

// Triangle-Triangle Overlap Test Routines
// July, 2002
// Updated December 2003
//
// This file contains C implementation of algorithms for
// performing two and three-dimensional triangle-triangle intersection test
// The algorithms and underlying theory are described in
//
// "Fast and Robust Triangle-Triangle Overlap Test
// Using Orientation Predicates"  P. Guigue - O. Devillers
//
// Journal of Graphics Tools, 8(1), 2003
//
// Several geometric predicates are defined.  Their parameters are all
// points.  Each point is an array of two or three double precision
// floating point numbers. The geometric predicates implemented in
// this file are:
//
//    int tri_tri_overlap_test_3d(p1,q1,r1,p2,q2,r2)
//    int tri_tri_overlap_test_2d(p1,q1,r1,p2,q2,r2)
//
//	int tri_tri_intersection_test_3d(p1,q1,r1,p2,q2,r2,
//                                    coplanar,source,target)
//
// is a version that computes the segment of intersection when
// the triangles overlap (and are not coplanar)
//
// each function returns 1 if the triangles (including their
// boundary) intersect, otherwise 0
//
//
// Other information are available from the Web page
// http://www.acm.org/jgt/papers/GuigueDevillers03/

static Vector3 N1, N2, source, target;
static bool coplanar = false;

// DONE
bool CHECK_MIN_MAX(Vector3 p1, Vector3 q1, Vector3 r1, Vector3 p2, Vector3 q2, Vector3 r2)
{
	Vector3 v1 = p2 - q1;
	Vector3 v2 = p1 - q1;

	Vector3 N1 = glm::cross(v1, v2);

	v1 = q2 - q1;
	if (glm::dot(v1, N1) > 0.0f)
	{
		return false;
	}

	v1 = p2 - p1;
	v2 = r1 - p1;
	N1 = glm::cross(v1, v2);
	v1 = r2 - p1;

	if (glm::dot(v1, N1) > 0.0f) {
		return false;
	}

	return true; 
}

// Permutation in a canonical form of T2's vertices
bool Physics::TRI_TRI_3D(Vector3 p1, Vector3 q1, Vector3 r1, Vector3 p2, Vector3 q2, Vector3 r2, double dp2, double dq2, double dr2) {
	  if (dp2 > 0.0f) {
			if (dq2 > 0.0f) CHECK_MIN_MAX(p1, r1, q1, r2, p2, q2);
			else if (dr2 > 0.0f) CHECK_MIN_MAX(p1,r1,q1,q2,r2,p2);
			else CHECK_MIN_MAX(p1,q1,r1,p2,q2,r2);
	  }
	  else if (dp2 < 0.0f) 
	  {
		if (dq2 < 0.0f) CHECK_MIN_MAX(p1,q1,r1,r2,p2,q2);
		else if (dr2 < 0.0f) CHECK_MIN_MAX(p1,q1,r1,q2,r2,p2);
		else CHECK_MIN_MAX(p1,r1,q1,p2,q2,r2);
	  } else {
			if (dq2 < 0.0f) {
				if (dr2 >= 0.0f) CHECK_MIN_MAX(p1, r1, q1, q2, r2, p2);
				else CHECK_MIN_MAX(p1,q1,r1,p2,q2,r2);
			}
			else if (dq2 > 0.0f) {
				if (dr2 > 0.0f) CHECK_MIN_MAX(p1, r1, q1, p2, q2, r2);
				else CHECK_MIN_MAX(p1,q1,r1,q2,r2,p2);
			}
			else  {
				if (dr2 > 0.0f) CHECK_MIN_MAX(p1,q1,r1,r2,p2,q2);
				else if (dr2 < 0.0f) CHECK_MIN_MAX(p1,r1,q1,r2,p2,q2);
				else return coplanar_tri_tri3d(p1,q1,r1,p2,q2,r2, N1, N2);
			}
	  }
}

// Three-dimensional Triangle-Triangle Overlap Test
bool Physics::tri_tri_overlap_test_3d(Vector3 p1, Vector3 q1, Vector3 r1,
	Vector3 p2, Vector3 q2, Vector3 r2)
{
	coplanar = false;

	N1 = Vector3(0, 0, 0);
	N2 = Vector3(0, 0, 0);

	source = Vector3(0, 0, 0);
	target = Vector3(0, 0, 0);

	double dp1, dq1, dr1, dp2, dq2, dr2;
	Vector3 v1, v2;

	// Compute distance signs  of p1, q1 and r1 to the plane of triangle(p2,q2,r2)
	v1 = p2 - r2;
	v2 = q2 - r2;
	Vector3 N2 = glm::cross(v1, v2);

	v1 = p1 - r2;
	dp1 = glm::dot(v1, N2);
	v1 = q1 - r2;
	dq1 = glm::dot(v1, N2);
	v1 = r1 - r2;
	dr1 = glm::dot(v1, N2);

	if (((dp1 * dq1) > 0.0f) && ((dp1 * dr1) > 0.0f)) 
	{
		return false;
	}

	// Compute distance signs  of p2, q2 and r2 to the plane of triangle(p1,q1,r1)
	v1 = q1 - p1;
	v2 = r1 - p1;
	N1 = glm::cross(v1, v2);

	v1 = p2 - r1;
	dp2 = glm::dot(v1, N1);
	v1 = q2 - r1;
	dq2 = glm::dot(v1, N1);
	v1 = r2 - r1;
	dr2 = glm::dot(v1, N1);

	if (((dp2 * dq2) > 0.0f) && ((dp2 * dr2) > 0.0f)) 
	{ 
		return false; 
	}

	if (dp1 > 0.0f) 
	{
		if (dq1 > 0.0f) TRI_TRI_3D(r1, p1, q1, p2, r2, q2, dp2, dr2, dq2);
		else if (dr1 > 0.0f) TRI_TRI_3D(q1, r1, p1, p2, r2, q2, dp2, dr2, dq2);
		else TRI_TRI_3D(p1, q1, r1, p2, q2, r2, dp2, dq2, dr2);
	}
	else if (dp1 < 0.0f) {
		if (dq1 < 0.0f) TRI_TRI_3D(r1, p1, q1, p2, q2, r2, dp2, dq2, dr2);
		else if (dr1 < 0.0f) TRI_TRI_3D(q1, r1, p1, p2, q2, r2, dp2, dq2, dr2);
		else TRI_TRI_3D(p1, q1, r1, p2, r2, q2, dp2, dr2, dq2);
	}
	else {
		if (dq1 < 0.0f) {
			if (dr1 >= 0.0f) TRI_TRI_3D(q1, r1, p1, p2, r2, q2, dp2, dr2, dq2);
			else TRI_TRI_3D(p1, q1, r1, p2, q2, r2, dp2, dq2, dr2);
		}
		else if (dq1 > 0.0f) {
			if (dr1 > 0.0f) TRI_TRI_3D(p1, q1, r1, p2, r2, q2, dp2, dr2, dq2);
			else TRI_TRI_3D(q1, r1, p1, p2, q2, r2, dp2, dq2, dr2);
		}
		else {
			if (dr1 > 0.0f) TRI_TRI_3D(r1, p1, q1, p2, q2, r2, dp2, dq2, dr2);
			else if (dr1 < 0.0f) TRI_TRI_3D(r1, p1, q1, p2, r2, q2, dp2, dr2, dq2);
			else return coplanar_tri_tri3d(p1, q1, r1, p2, q2, r2, N1, N2);
		}
	}
};

bool Physics::coplanar_tri_tri3d(Vector3 p1, Vector3 q1, Vector3 r1,
	Vector3 p2, Vector3 q2, Vector3 r2,
	Vector3 normal_1, Vector3 normal_2) {

	Vector2 P1, Q1, R1;
	Vector2 P2, Q2, R2;

	double n_x, n_y, n_z;

	n_x = ((normal_1[0] < 0) ? -normal_1[0] : normal_1[0]);
	n_y = ((normal_1[1] < 0) ? -normal_1[1] : normal_1[1]);
	n_z = ((normal_1[2] < 0) ? -normal_1[2] : normal_1[2]);

	// Projection of the triangles in 3D onto 2D such that the area of the projection is maximized.
	if ((n_x > n_z) && (n_x >= n_y)) {
		// Project onto plane YZ
		P1[0] = q1[2]; P1[1] = q1[1];
		Q1[0] = p1[2]; Q1[1] = p1[1];
		R1[0] = r1[2]; R1[1] = r1[1];

		P2[0] = q2[2]; P2[1] = q2[1];
		Q2[0] = p2[2]; Q2[1] = p2[1];
		R2[0] = r2[2]; R2[1] = r2[1];

	}
	else if ((n_y > n_z) && (n_y >= n_x)) {
		// Project onto plane XZ
		P1[0] = q1[0]; P1[1] = q1[2];
		Q1[0] = p1[0]; Q1[1] = p1[2];
		R1[0] = r1[0]; R1[1] = r1[2];

		P2[0] = q2[0]; P2[1] = q2[2];
		Q2[0] = p2[0]; Q2[1] = p2[2];
		R2[0] = r2[0]; R2[1] = r2[2];
	}
	else {
		// Project onto plane XY
		P1[0] = p1[0]; P1[1] = p1[1];
		Q1[0] = q1[0]; Q1[1] = q1[1];
		R1[0] = r1[0]; R1[1] = r1[1];

		P2[0] = p2[0]; P2[1] = p2[1];
		Q2[0] = q2[0]; Q2[1] = q2[1];
		R2[0] = r2[0]; R2[1] = r2[1];
	}

	return tri_tri_overlap_test_2d(P1, Q1, R1, P2, Q2, R2);
};

// Three-dimensional Triangle-Triangle Intersection

// is called when the triangles surely intersect
// It constructs the segment of intersection of the two triangles
// if they are not coplanar.
bool CONSTRUCT_INTERSECTION(Vector3 p1, Vector3 q1, Vector3 r1, Vector3 p2, Vector3 q2, Vector3 r2) {
	Vector3 v1 = q1 - p1;
	Vector3 v2 = r2 - p1;
	Vector3 N = glm::cross(v1, v2);
	Vector3 v = p2 - p1;

	if (glm::dot(v, N) > 0.0f) {
		v1 = r1 - p1;
		N = glm::cross(v1, v2);

		if (glm::dot(v, N) <= 0.0f) {
			v2 = q2 - p1;
			N = glm::cross(v1, v2);

			if (glm::dot(v, N) > 0.0f) {
				v1 = p1 - p2;
				v2 = p1 - r1;
				float alpha = glm::dot(v1, N2) / glm::dot(v2, N2);
				v1 = alpha * v2;
				source = p1 - v1;
				v1 = p2 - p1;
				v2 = p2 - r2;
				alpha = glm::dot(v1, N1) / glm::dot(v2, N1);
				v1 = alpha * v2;
				Vector3 target = p2 - v1;

				return true;
			} else {
				v1 = p2 - p1;
				v2 = p2 - q2;
				float alpha = glm::dot(v1, N1) / glm::dot(v2, N1);
				v1 = alpha * v2;
				source = p2 - v1;
				v1 = p2 - p1;
				v2 = p2 - r2;
				alpha = glm::dot(v1, N1) / glm::dot(v2, N1);
				v1 = alpha * v2;
				Vector3 target = p2 - v1;

				return true;
			}
		} else {
			return false;
		}
	} else {
		v2 = q2 - p1;
		N = glm::cross(v1, v2);

		if (glm::dot(v, N) < 0.0f) {
			return false;
		} else {
			v1 = r1 - p1;
			N = glm::cross(v1, v2);

			if (glm::dot(v, N) >= 0.0f) {
				v1 = p1 - p2;
				v2 = p1 - r1;
				float alpha = glm::dot(v1, N2) / glm::dot(v2, N2);
				v1 = alpha * v2;
				source = p1 - v1;
				v1 = p1 - p2;
				v2 = p1 - q1;
				alpha = glm::dot(v1, N2) / glm::dot(v2, N2);
				v1 = alpha * v2;
				target = p1 - v1;

				return true;	  
			} else {
				v1 = p2 - p1;
				v2 = p2 - q2;
				float alpha = glm::dot(v1, N1) / glm::dot(v2, N1);
				v1 = alpha * v2;
				source = p2 - v1;
				v1 = p1 - p2;
				v2 = p1 - q1;
				alpha = glm::dot(v1, N2) / glm::dot(v2, N2);
				v1 = alpha * v2;
				target = p1 - v1;

				return true;
			}
		}
	}
} 

bool Physics::TRI_TRI_INTER_3D(Vector3 p1, Vector3 q1, Vector3 r1, Vector3 p2, Vector3 q2, Vector3 r2, double dp2, double dq2, double dr2)
{
	if (dp2 > 0.0f) {
		if (dq2 > 0.0f) CONSTRUCT_INTERSECTION(p1,r1,q1,r2,p2,q2);
		else if (dr2 > 0.0f) CONSTRUCT_INTERSECTION(p1,r1,q1,q2,r2,p2);
		else CONSTRUCT_INTERSECTION(p1, q1, r1, p2, q2, r2);
	}
	else if (dp2 < 0.0f) {
		if (dq2 < 0.0f) CONSTRUCT_INTERSECTION(p1,q1,r1,r2,p2,q2);
		else if (dr2 < 0.0f) CONSTRUCT_INTERSECTION(p1,q1,r1,q2,r2,p2);
		else CONSTRUCT_INTERSECTION(p1,r1,q1,p2,q2,r2);
	} else {
		if (dq2 < 0.0f) {
			if (dr2 >= 0.0f)  CONSTRUCT_INTERSECTION(p1,r1,q1,q2,r2,p2);
			else CONSTRUCT_INTERSECTION(p1,q1,r1,p2,q2,r2);
		}
		else if (dq2 > 0.0f) {
			if (dr2 > 0.0f) CONSTRUCT_INTERSECTION(p1,r1,q1,p2,q2,r2);
			else  CONSTRUCT_INTERSECTION(p1, q1, r1, q2, r2, p2);
		}
		else  {
			if (dr2 > 0.0f) CONSTRUCT_INTERSECTION(p1,q1,r1,r2,p2,q2);
			else if (dr2 < 0.0f) CONSTRUCT_INTERSECTION(p1, r1, q1, r2, p2, q2);
			else {
				coplanar = true;
				return coplanar_tri_tri3d(p1,q1,r1,p2,q2,r2, N1, N2);
			}
		}
	} 
}

// The following version computes the segment of intersection of the
// two triangles if it exists.
// coplanar returns whether the triangles are coplanar
// source and target are the endpoints of the line segment of intersection
bool Physics::tri_tri_intersection_test_3d(Vector3 p1, Vector3 q1, Vector3 r1,
	Vector3 p2, Vector3 q2, Vector3 r2,
	int* coplanar,
	Vector3 source, Vector3 target)
{
	double dp1, dq1, dr1, dp2, dq2, dr2;
	Vector3 v1, v2, v;
	Vector3 N1, N2, N;
	double alpha;

	// Compute distance signs  of p1, q1 and r1 
	// to the plane of triangle(p2,q2,r2)
	v1 = p2 - r2;
	v2 = q2 - r2;
	N2 = glm::cross(v1, v2);

	v1 = p1 - r2;
	dp1 = glm::dot(v1, N2);
	v1 = q1 - r2;
	dq1 = glm::dot(v1, N2);
	v1 = r1 - r2;
	dr1 = glm::dot(v1, N2);

	if (((dp1 * dq1) > 0.0f) && ((dp1 * dr1) > 0.0f)) 
	{
		return false;
	}

	// Compute distance signs  of p2, q2 and r2 
	// to the plane of triangle(p1,q1,r1)
	v1 = q1 - p1;
	v2 = r1 - p1;
	N1 = glm::cross(v1, v2);

	v1 = p2 - r1;
	dp2 = glm::dot(v1, N1);
	v1 = q2 - r1;
	dq2 = glm::dot(v1, N1);
	v1 = r2 - r1;
	dr2 = glm::dot(v1, N1);

	if (((dp2 * dq2) > 0.0f) && ((dp2 * dr2) > 0.0f)) 
	{
		return false;
	}

	// Permutation in a canonical form of T1's vertices
	if (dp1 > 0.0f) {
		if (dq1 > 0.0f) TRI_TRI_INTER_3D(r1, p1, q1, p2, r2, q2, dp2, dr2, dq2);
		else if (dr1 > 0.0f) TRI_TRI_INTER_3D(q1, r1, p1, p2, r2, q2, dp2, dr2, dq2);
		else TRI_TRI_INTER_3D(p1, q1, r1, p2, q2, r2, dp2, dq2, dr2);
	}
	else if (dp1 < 0.0f) {
		if (dq1 < 0.0f) TRI_TRI_INTER_3D(r1, p1, q1, p2, q2, r2, dp2, dq2, dr2);
		else if (dr1 < 0.0f) TRI_TRI_INTER_3D(q1, r1, p1, p2, q2, r2, dp2, dq2, dr2);
		else TRI_TRI_INTER_3D(p1, q1, r1, p2, r2, q2, dp2, dr2, dq2);
	}
	else {
		if (dq1 < 0.0f) {
			if (dr1 >= 0.0f) TRI_TRI_INTER_3D(q1, r1, p1, p2, r2, q2, dp2, dr2, dq2);
			else TRI_TRI_INTER_3D(p1, q1, r1, p2, q2, r2, dp2, dq2, dr2);
		}
		else if (dq1 > 0.0f) {
			if (dr1 > 0.0f) TRI_TRI_INTER_3D(p1, q1, r1, p2, r2, q2, dp2, dr2, dq2);
			else TRI_TRI_INTER_3D(q1, r1, p1, p2, q2, r2, dp2, dq2, dr2);
		}
		else {
			if (dr1 > 0.0f) TRI_TRI_INTER_3D(r1, p1, q1, p2, q2, r2, dp2, dq2, dr2);
			else if (dr1 < 0.0f) TRI_TRI_INTER_3D(r1, p1, q1, p2, r2, q2, dp2, dr2, dq2);
			else {
				// triangles are co-planar

				*coplanar = 1;
				return coplanar_tri_tri3d(p1, q1, r1, p2, q2, r2, N1, N2);
			}
		}
	}
};




// Two dimensional Triangle-Triangle Overlap Test


// Some 2D functions
float ORIENT_2D(Vector2 a, Vector2 b, Vector2 c) 
{  
	return ((a[0] - c[0]) * (b[1] - c[1]) - (a[1] - c[1]) * (b[0] - c[0]));
}

bool INTERSECTION_TEST_VERTEX(Vector2 P1, Vector2 Q1, Vector2 R1, Vector2 P2, Vector2 Q2, Vector2 R2) {
  if (ORIENT_2D(R2,P2,Q1) >= 0.0f)
    if (ORIENT_2D(R2,Q2,Q1) <= 0.0f)
      if (ORIENT_2D(P1,P2,Q1) > 0.0f) {
  if (ORIENT_2D(P1,Q2,Q1) <= 0.0f) return 1;
  else return 0;} else {
  if (ORIENT_2D(P1,P2,R1) >= 0.0f)
    if (ORIENT_2D(Q1,R1,P2) >= 0.0f) return 1;
    else return 0;
  else return 0;}
    else 
      if (ORIENT_2D(P1,Q2,Q1) <= 0.0f)
  if (ORIENT_2D(R2,Q2,R1) <= 0.0f)
    if (ORIENT_2D(Q1,R1,Q2) >= 0.0f) return 1;
    else return 0;
  else return 0;
      else return 0;
  else
    if (ORIENT_2D(R2,P2,R1) >= 0.0f)
      if (ORIENT_2D(Q1,R1,R2) >= 0.0f)
  if (ORIENT_2D(P1,P2,R1) >= 0.0f) return 1;
  else return 0;
      else
  if (ORIENT_2D(Q1,R1,Q2) >= 0.0f) {
    if (ORIENT_2D(R2,R1,Q2) >= 0.0f) return 1;
    else return 0; }
  else return 0;
    else  return 0;
 };



bool INTERSECTION_TEST_EDGE(Vector2 P1, Vector2 Q1, Vector2 R1, Vector2 P2, Vector2 Q2, Vector2 R2) {
	if (ORIENT_2D(R2,P2,Q1) >= 0.0f) 
	{
		if (ORIENT_2D(P1,P2,Q1) >= 0.0f) 
		{
			if (ORIENT_2D(P1, Q1, R2) >= 0.0f) { return true; } else { return false; }
		} 
		else 
		{
			if (ORIENT_2D(Q1,R1,P2) >= 0.0f)
			{
				if (ORIENT_2D(R1, P1, P2) >= 0.0f) { return true; } else { return false; }
			}
			else 
			{ 
				return false; 
			}
		}
	} 
	else 
	{
		if (ORIENT_2D(R2,P2,R1) >= 0.0f) 
		{
			if (ORIENT_2D(P1,P2,R1) >= 0.0f) 
			{
				if (ORIENT_2D(P1, R1, R2) >= 0.0f) 
				{ 
					return true; 
				}
				else 
				{
					if (ORIENT_2D(Q1, R1, R2) >= 0.0f) { return true; } else { return false; }
				}
			}
			else 
			{ 
				return false; 
			}
		}
		else 
		{ 
			return false; 
		}
	}
}

bool ccw_tri_tri_intersection_2d(Vector2 p1, Vector2 q1, Vector2 r1,
	Vector2 p2, Vector2 q2, Vector2 r2) {
	if (ORIENT_2D(p2, q2, p1) >= 0.0f) {
		if (ORIENT_2D(q2, r2, p1) >= 0.0f) {
			if (ORIENT_2D(r2, p2, p1) >= 0.0f) return true;
			else INTERSECTION_TEST_EDGE(p1, q1, r1, p2, q2, r2);
		}
		else {
			if (ORIENT_2D(r2, p2, p1) >= 0.0f)
				INTERSECTION_TEST_EDGE(p1, q1, r1, r2, p2, q2);
			else INTERSECTION_TEST_VERTEX(p1, q1, r1, p2, q2, r2);
		}
	}
	else {
		if (ORIENT_2D(q2, r2, p1) >= 0.0f) {
			if (ORIENT_2D(r2, p2, p1) >= 0.0f)
				INTERSECTION_TEST_EDGE(p1, q1, r1, q2, r2, p2);
			else  INTERSECTION_TEST_VERTEX(p1, q1, r1, q2, r2, p2);
		}
		else INTERSECTION_TEST_VERTEX(p1, q1, r1, r2, p2, q2);
	}
};


bool Physics::tri_tri_overlap_test_2d(Vector2 p1, Vector2 q1, Vector2 r1,
	Vector2 p2, Vector2 q2, Vector2 r2) 
{
	if (ORIENT_2D(p1, q1, r1) < 0.0f) {
		if (ORIENT_2D(p2, q2, r2) < 0.0f)
			return ccw_tri_tri_intersection_2d(p1, r1, q1, p2, r2, q2);
		else
			return ccw_tri_tri_intersection_2d(p1, r1, q1, p2, q2, r2);
	}
	else 
	{
		if (ORIENT_2D(p2, q2, r2) < 0.0f)
			return ccw_tri_tri_intersection_2d(p1, q1, r1, p2, r2, q2);
		else
			return ccw_tri_tri_intersection_2d(p1, q1, r1, p2, q2, r2);
	}
};