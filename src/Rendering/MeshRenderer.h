#pragma once

#include "Rendering\Renderer.h"

#include "glm\glm.hpp"

#include "Rendering\Material\Material.h"

#include "assimp\Importer.hpp"
#include "assimp\scene.h"

#include "Rendering\Mesh.h"

#include "Physics\BoundingBox.h"

namespace Graphyte {
	class Camera;

	class MeshRenderer : public Renderer {
	private:
		void processMesh(const aiMesh* mesh, const aiScene* scene, const std::string directory);
	public:
		Mesh mesh;

		float specularIntensity = 0;
		float specularPower = 32;
		Vector3 objectColor = Vector3(1, 1, 1);

		// Constructor
		MeshRenderer();
		MeshRenderer(const aiMesh* mesh, const aiScene* scene, const std::string directory);

		void Render(Camera& camera);
		void RenderExtras();
		void OnComponentAdded();

		void RecalculateBoundingBox();

		void DrawLines();
		void DrawLine(const Vector3 color);
		void DrawUI();
	};
}