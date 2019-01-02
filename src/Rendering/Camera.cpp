#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

Camera *Camera::mainCamera;

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix()
{
	// TODO: Fixing transform to work with this
	glm::vec3 position = glm::vec3(transform->position.x, transform->position.y, transform->position.z);
	glm::vec3 upVector = glm::vec3(transform->getUpVector().x, transform->getUpVector().y, transform->getUpVector().z);
	glm::vec3 forwardVector = glm::vec3(transform->getForwardVector().x, transform->getForwardVector().y, transform->getForwardVector().z);

	return glm::lookAt(position, position + forwardVector, upVector);
}
