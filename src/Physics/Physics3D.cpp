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
