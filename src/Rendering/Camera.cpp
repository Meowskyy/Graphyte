#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "Rendering/Screen.h"

using namespace Graphyte;

Camera* Camera::mainCamera;

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
Matrix4 Camera::GetViewMatrix()
{
	Vector3 position = Vector3(transform->position.x, transform->position.y, transform->position.z);
	Vector3 upVector = Vector3(transform->getUpVector().x, transform->getUpVector().y, transform->getUpVector().z);
	Vector3 forwardVector = Vector3(transform->getForwardVector().x, transform->getForwardVector().y, transform->getForwardVector().z);

	return glm::lookAt(position, position + forwardVector, upVector);
}

Matrix4 Camera::GetProjectionMatrix()
{
	return 	glm::perspective(glm::radians(fov), (float)Screen::width / (float)Screen::height, nearClipPlane, farClipPlane);
}
