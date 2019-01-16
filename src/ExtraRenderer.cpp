#include "ExtraRenderer.h"

#include "Resources\ResourceManager.h"
#include "Objects\Scene.h"

#include "GLFW\glfw3.h"
#include "glm\vec4.hpp"

#include <vector>

#include "Rendering\MeshRenderer.h"

void ExtraRenderer::DrawAABB(BoundingBox& box, Vector3& position) {
	// Cube 1x1x1, centered on origin
	float vertices[] = {
		-0.5, -0.5, -0.5,
		 0.5, -0.5, -0.5,
		 0.5,  0.5, -0.5,
		-0.5,  0.5, -0.5,
		-0.5, -0.5,  0.5,
		 0.5, -0.5,  0.5,
		 0.5,  0.5,  0.5,
		-0.5,  0.5,  0.5,
	};

	unsigned int indices[] = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		0, 4, 1, 5,
		2, 6, 3, 7
	};
	
	MeshRenderer meshRenderer = MeshRenderer();

	std::vector<Vector3> newVertices;
	std::vector<unsigned int> newIndices;

	for (int i = 0; i < sizeof(vertices); i += 3) {
		newVertices.push_back(Vector3(vertices[i], vertices[i + 1], vertices[i + 2]));
	}

	for (int i = 0; i < sizeof(indices); i++) {
		newIndices.push_back(indices[i]);
	}

	meshRenderer.mesh.vertices = newVertices;
	meshRenderer.mesh.indices = newIndices;

	// TODO: Calculate bounding box based on rotation
	float width = box.max.x - box.min.x;
	float height = box.max.y - box.min.y;
	float depth = box.max.z - box.min.z;

	meshRenderer.transform->position = position + Vector3(box.min.x + width / 2, box.min.y + height / 2, box.min.z + depth / 2);

	meshRenderer.transform->scale = Vector3(width, height, depth);

	meshRenderer.OnComponentAdded();

	meshRenderer.DrawLines();
}

void ExtraRenderer::DrawUniformBox(BoundingBox& box) {
	// Cube 1x1x1, centered on origin
	float vertices[] = {
		-0.5, -0.5, -0.5,
		 0.5, -0.5, -0.5,
		 0.5,  0.5, -0.5,
		-0.5,  0.5, -0.5,
		-0.5, -0.5,  0.5,
		 0.5, -0.5,  0.5,
		 0.5,  0.5,  0.5,
		-0.5,  0.5,  0.5,
	};

	unsigned int indices[] = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		0, 4, 1, 5,
		2, 6, 3, 7
	};

	MeshRenderer meshRenderer = MeshRenderer();

	std::vector<Vector3> newVertices;
	std::vector<unsigned int> newIndices;

	for (int i = 0; i < sizeof(vertices); i += 3) {
		newVertices.push_back(Vector3(vertices[i], vertices[i + 1], vertices[i + 2]));
	}

	for (int i = 0; i < sizeof(indices); i++) {
		newIndices.push_back(indices[i]);
	}

	meshRenderer.mesh.vertices = newVertices;
	meshRenderer.mesh.indices = newIndices;

	float size = box.max.x - box.min.x;

	meshRenderer.transform->position = Vector3(box.min.x + size / 2, box.min.y + size / 2, (box.min.z + size / 2));

	meshRenderer.transform->scale = Vector3(size, size, size);

	meshRenderer.OnComponentAdded();

	meshRenderer.DrawLines();
}