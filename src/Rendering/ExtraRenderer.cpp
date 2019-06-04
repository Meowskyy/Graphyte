#include "ExtraRenderer.h"

#include "Resources\ResourceManager.h"
#include "Objects\Scene.h"

#include "GLFW\glfw3.h"
#include "glm\vec4.hpp"

#include <vector>

#include "Rendering\MeshRenderer.h"

using namespace Graphyte;

void ExtraRenderer::DrawAABB(const BoundingBox& box, const Vector3& position) {

	MeshRenderer meshRenderer = MeshRenderer();

	meshRenderer.mesh.vertices = std::vector<Vector3>
	{
		Vector3(-0.5, -0.5, -0.5),
		Vector3(0.5, -0.5, -0.5),
		Vector3(0.5,  0.5, -0.5),
		Vector3(-0.5,  0.5, -0.5),
		Vector3(-0.5, -0.5,  0.5),
		Vector3(0.5, -0.5,  0.5),
		Vector3(0.5,  0.5,  0.5),
		Vector3(-0.5,  0.5,  0.5),
	};

	meshRenderer.mesh.indices = std::vector<unsigned int>
	{
		0, 1, 2, 3,
		4, 5, 6, 7,
		0, 4, 1, 5,
		2, 6, 3, 7
	};

	// TODO: Calculate bounding box based on rotation
	float width = box.max.x - box.min.x;
	float height = box.max.y - box.min.y;
	float depth = box.max.z - box.min.z;

	meshRenderer.transform->position = position + Vector3(box.min.x + width / 2, box.min.y + height / 2, box.min.z + depth / 2);

	meshRenderer.transform->scale = Vector3(width, height, depth);

	meshRenderer.OnComponentAdded();

	ResourceManager::GetShader("Unlit").SetVector3f("Color", Vector3(1, 1, 1), true);
	meshRenderer.DrawLines();
}

void ExtraRenderer::DrawUniformBox(const BoundingBox& box) {
	MeshRenderer meshRenderer = MeshRenderer();

	meshRenderer.mesh.vertices = std::vector<Vector3>
	{
		Vector3(-0.5, -0.5, -0.5),
		Vector3(0.5, -0.5, -0.5),
		Vector3(0.5,  0.5, -0.5),
		Vector3(-0.5,  0.5, -0.5),
		Vector3(-0.5, -0.5,  0.5),
		Vector3(0.5, -0.5,  0.5),
		Vector3(0.5,  0.5,  0.5),
		Vector3(-0.5,  0.5,  0.5),
	};

	meshRenderer.mesh.indices = std::vector<unsigned int>
	{ 
		0, 1, 2, 3,
		4, 5, 6, 7,
		0, 4, 1, 5,
		2, 6, 3, 7
	};

	float size = box.max.x - box.min.x;

	meshRenderer.transform->position = Vector3(box.min.x + size / 2, box.min.y + size / 2, (box.min.z + size / 2));
	meshRenderer.transform->scale = Vector3(size, size, size);

	meshRenderer.OnComponentAdded();

	ResourceManager::GetShader("Unlit").SetVector3f("Color", Vector3(1, 1, 1), true);
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
	DrawLine(transf->position, transf->getForwardVector() * distance, Vector3(0, 1, 0));
	DrawLine(transf->position, transf->getRightVector()* distance, Vector3(1, 0, 0));
	DrawLine(transf->position, transf->getUpVector()* distance, Vector3(0, 0, 1));
}
