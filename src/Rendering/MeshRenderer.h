#pragma once

#include "ECS.h"

#include "glm\glm.hpp"

#include "Rendering\Material\Material.h"

#include "assimp\Importer.hpp"
#include "assimp\scene.h"

#include "Rendering\Mesh.h"

#include "BoundingBox.h"

class MeshRenderer : public Component {
private:
	void processMesh(const aiMesh* mesh, const aiScene* scene, const std::string directory);
public:
	// TODO: Remove this from release
	bool drawBoundingBox = false;
	BoundingBox boundaries;

	Mesh mesh;

	std::vector<Material> materials;

	// Constructor
	MeshRenderer();
	MeshRenderer(const aiMesh* mesh, const aiScene* scene, const std::string directory);

	void Update();
	void OnComponentAdded();

	void RecalculateBoundingBox();

	void DrawLines();
	void DrawUI();
};