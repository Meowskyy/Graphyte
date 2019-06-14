#include "ExtraRenderer.h"

#include "Resources\ResourceManager.h"
#include "Objects\Scene.h"

#include "GLFW\glfw3.h"
#include "glm\vec4.hpp"

#include <vector>

#include "Rendering\MeshRenderer.h"

using namespace Graphyte;

void ExtraRenderer::DrawAABB(const AxisAlignedBoundingBox& box) 
{
	MeshRenderer meshRenderer = MeshRenderer();

	meshRenderer.mesh.vertices = std::vector<Vector3>
	{
		Vector3(box.min.x, box.min.y,  box.max.z),	// OK
		Vector3(box.max.x, box.min.y,  box.max.z),	// OK
		box.max,									// OK
		Vector3(box.min.x, box.max.y,  box.max.z),	// OK

		box.min,									// OK
		Vector3(box.max.x, box.min.y, box.min.z),	// OK
		Vector3(box.max.x, box.max.y, box.min.z),	// OK
		Vector3(box.min.x, box.max.y, box.min.z)	// OK
	};

	meshRenderer.mesh.indices = std::vector<unsigned int>
	{
		0, 1, 2, 3,
		4, 5, 6, 7,
		0, 4, 1, 5,
		2, 6, 3, 7
	};

	meshRenderer.transform->position = box.transform->position;

	meshRenderer.OnComponentAdded();

	ResourceManager::GetShader("Unlit").SetVector3f("Color", Vector3(1, 1, 1), true);

	ResourceManager::GetShader("Unlit").SetMatrix4("view", Camera::mainCamera->GetViewMatrix());

	meshRenderer.DrawLines();
}

void ExtraRenderer::DrawBounds(const Bounds& box) {
	MeshRenderer meshRenderer = MeshRenderer();

	meshRenderer.mesh.vertices = std::vector<Vector3>
	{
		Vector3(box.min.x, box.min.y,  box.max.z),	// OK
		Vector3(box.max.x, box.min.y,  box.max.z),	// OK
		box.max,									// OK
		Vector3(box.min.x, box.max.y,  box.max.z),	// OK

		box.min,											// OK
		Vector3(box.max.x, box.min.y, box.min.z),	// OK
		Vector3(box.max.x, box.max.y, box.min.z),	// OK
		Vector3(box.min.x, box.max.y, box.min.z)	// OK
	};

	meshRenderer.mesh.indices = std::vector<unsigned int>
	{
		0, 1, 2, 3,
		4, 5, 6, 7,
		0, 4, 1, 5,
		2, 6, 3, 7
	};

	meshRenderer.OnComponentAdded();

	ResourceManager::GetShader("Unlit").SetVector3f("Color", Vector3(1, 1, 1), true);
	ResourceManager::GetShader("Unlit").SetMatrix4("view", Camera::mainCamera->GetViewMatrix());

	meshRenderer.DrawLines();
}

void ExtraRenderer::DrawLine(const Vector3& start, const Vector3& end, const Vector3& color)
{
	MeshRenderer meshRenderer = MeshRenderer();

	meshRenderer.mesh.vertices = std::vector<Vector3> 
	{
		start,
		start + end
	};

	meshRenderer.mesh.indices = std::vector<unsigned int>
	{
		0, 1
	};

	meshRenderer.OnComponentAdded();

	ResourceManager::GetShader("Unlit").SetVector3f("Color", color, true);
	meshRenderer.DrawLine(color);
}

void ExtraRenderer::DrawSelectionArrows(const Transform* transf, const float distance)
{
	glDisable(GL_DEPTH_TEST);
	DrawLine(transf->position, transf->GetForwardVector() * distance, Vector3(0, 1, 0));
	DrawLine(transf->position, transf->GetRightVector()* distance, Vector3(1, 0, 0));
	DrawLine(transf->position, transf->GetUpVector()* distance, Vector3(0, 0, 1));
}
