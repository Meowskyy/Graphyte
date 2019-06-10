#include "Camera.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "Rendering/Screen.h"

using namespace Graphyte;

Camera* Camera::mainCamera;

bool Camera::IsTransformInView(const Transform& transform) const
{
	// Is inside the frustrum
	bool result = true;

	for (int i = 0; i < 6; i++)
	{
		const float pos = frustrum.planes[i].w;
		const Vector3 normal = Vector3(frustrum.planes[i]);

		if (glm::dot(normal, transform.boundingBox.getPositiveVertex(normal) + transform.position) + pos < 0.0f)
		{
			return false;
		}

		if (glm::dot(normal, transform.boundingBox.getNegativeVertex(normal) + transform.position) + pos < 0.0f)
		{
			result = true;
		}
	}

	return result;
}

// Returns the view matrix calculated using Euler Angles and the LookAt Matrix
Matrix4 Camera::GetViewMatrix() const
{
	Vector3 position = Vector3(transform->position.x, transform->position.y, transform->position.z);
	Vector3 upVector = Vector3(transform->GetUpVector().x, transform->GetUpVector().y, transform->GetUpVector().z);
	Vector3 forwardVector = Vector3(transform->GetForwardVector().x, transform->GetForwardVector().y, transform->GetForwardVector().z);

	return glm::lookAt(position, position + forwardVector, upVector);
}

Matrix4 Camera::GetProjectionMatrix() const
{
	return glm::perspective(glm::radians(fov), (float)Screen::width / (float)Screen::height, nearClipPlane, farClipPlane);
}

void Camera::DrawUI()
{
	ImGui::DragFloat("FoV", &fov);
	ImGui::DragFloat("Near Clip Plane", &nearClipPlane);
	ImGui::DragFloat("Far Clip Plane", &farClipPlane);
}
