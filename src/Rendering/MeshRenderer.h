#pragma once

#include "glm\glm.hpp"

#include "ECS.h"

#include "Rendering\Material\Material.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "Rendering\Mesh.h"

#include "BoundingBox.h"

class MeshRenderer : public Component {
private:
	void processMesh(const aiMesh* mesh, const aiScene* scene, const std::string directory);

public:
	// TODO: Remove this from release
	bool drawBoundingBox = false;

	Mesh mesh;

	std::vector<Material> materials;

	// Constructor
	MeshRenderer();
	MeshRenderer(aiMesh* mesh, const aiScene* scene, const std::string directory);

	void Update();
	void OnComponentAdded();

	void DrawLines();

	void RecalculateBoundingBox();

	void DrawUI();
};