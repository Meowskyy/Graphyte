#pragma once

#include "glm\glm.hpp"

#include "Scripting\BehaviourScript.h"

#include "Rendering\Material\Material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Rendering\Mesh.h"

#include "BoundingBox.h"

class MeshRenderer : public BehaviourScript {
private:
	void processMesh(const aiMesh* mesh, const aiScene* scene, const std::string directory);

	glm::mat4 model;
	glm::mat4 rot;
public:
	// TODO: Remove this from release
	bool drawBoundingBox = true;

	Mesh mesh;

	std::vector<Material> materials;

	// Constructor
	MeshRenderer();
	MeshRenderer(aiMesh* mesh, const aiScene* scene, const std::string directory);

	void Update();
	void OnBehaviourAdded();

	void DrawLines();

	void RecalculateBoundingBox();

	void DrawUI();
};