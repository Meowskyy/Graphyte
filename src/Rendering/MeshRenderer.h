#pragma once

#include "glm\glm.hpp"

#include "Scripting\BehaviourScript.h"

//#include "Material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Mesh.h"

class MeshRenderer : public BehaviourScript {
private:
	void processMesh(aiMesh *mesh, const aiScene *scene, std::string directory);

	Shader shader;
public:
	Mesh mesh;

	BoundingBox boundaries;

	//std::vector<Material> materials;

	// Constructor
	MeshRenderer(Transform &newTransform);
	MeshRenderer::MeshRenderer(Transform &newTransform, aiMesh* mesh, const aiScene* scene, std::string directory);

	void Update();
	void OnScriptAdd() {
		glm::vec3 dimensions = boundaries.max - boundaries.min;
		transform.position = boundaries.min + (dimensions / 2.0f);
	}
};