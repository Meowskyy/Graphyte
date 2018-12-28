#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

Camera *Camera::mainCamera;

// Constructor
Camera::Camera(Transform &newTransform)
	: BehaviourScript(newTransform)
{

}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(transform.position, transform.position + transform.getForwardVector(), transform.getUpVector());
}
