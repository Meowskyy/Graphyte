#include "Physics3D.h"

#include "Input\Input.h"

// https://github.com/capnramses/antons_opengl_tutorials_book/blob/master/07_ray_picking/main.cpp

/* takes mouse position on screen and return ray in world coords */
Vector3 Physics::RaycastMousePosition(const float distance) {
	Vector2 mousePos = Input::GetMousePosition();

	// TODO: Raycasting only works properly with 1920x1080 right now

	// screen space (viewport coordinates)
	float x = (2.0f * mousePos.x) / 1920 - 1.0f;
	float y = 1.0f - (2.0f * -mousePos.y) / 1080;
	float z = 1.0f;
	// normalised device space
	Vector3 ray_nds = Vector3(x, y, z);
	// clip space
	Vector4 ray_clip = Vector4(ray_nds.x, ray_nds.y, -1.0, 1.0);
	// eye space
	glm::mat4 projection = glm::perspective(glm::radians(70.0f), (float)1920 / (float)1080, 0.1f, 100.0f);
	Vector4 ray_eye = glm::inverse(projection) * ray_clip;
	ray_eye = Vector4(ray_eye.x, ray_eye.y, -1.0, 0.0);
	// world space
	glm::mat4 view = Camera::mainCamera->GetViewMatrix();
	Vector3 ray_wor = Vector3(glm::inverse(view) * ray_eye);
	// don't forget to normalise the vector at some point
	ray_wor = glm::normalize(ray_wor);
	return ray_wor * distance + Camera::mainCamera->transform->position + Camera::mainCamera->transform->getForwardVector();
}
