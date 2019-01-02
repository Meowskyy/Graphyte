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

	meshRenderer.mesh.setupMesh();

	meshRenderer.transform = new Transform();
	meshRenderer.transform->position = position + Vector3(0, box.max.y, 0);
	meshRenderer.transform->scale = Vector3(box.max.x - box.min.x, box.max.y - box.min.y, box.max.z - box.min.z);

	meshRenderer.OnBehaviourAdded();

	//glm::vec3 center = glm::vec3((box.min.x + box.max.x) / 2, (box.min.y + box.max.y) / 2, (box.min.z + box.max.z) / 2);
	//glm::mat4 transform = glm::translate(glm::mat4(1), center) * glm::scale(glm::mat4(1), size);

	meshRenderer.DrawLines();

	std::cout << "Drawing AABB" << std::endl;
}