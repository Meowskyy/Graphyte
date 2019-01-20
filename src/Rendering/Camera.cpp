#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

Camera* Camera::mainCamera;

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix()
{
	// TODO: Fixing transform to work with this
	Vector3 position = glm::vec3(transform->position.x, transform->position.y, transform->position.z);
	Vector3 upVector = glm::vec3(transform->getUpVector().x, transform->getUpVector().y, transform->getUpVector().z);
	Vector3 forwardVector = glm::vec3(transform->getForwardVector().x, transform->getForwardVector().y, transform->getForwardVector().z);

	return glm::lookAt(position, position + forwardVector, upVector);
}
