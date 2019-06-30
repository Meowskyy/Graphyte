#pragma once

#include "ECS.h"

#include "Rendering\Material\Material.h"

#include <map>

namespace Graphyte {
	class Camera;
	class ShadowMap;

	class Renderer : public Component {
	private:
		static std::map<std::string, std::vector<Renderer*>> renderers;
		virtual void Render(Camera& camera) {}
	public:
		bool drawBoundingBox = false;
		bool isVisible = false;
		bool drawMesh = true;
		Bounds boundaries;

		std::vector<Material> materials;

		// Constructor
		Renderer();

		// Public functions
		void SetMaterial(Material material);

		// EXPERIMENTAL
		static void RenderAllGrouped(Camera& camera);
		static void RenderAllDepth(Camera& camera);
		static void RenderAllWithShader(Camera& camera, Material& mat);

		// EXTRA
		void DrawLines() {}
		void DrawLine(const Vector3 color) {}
		void DrawUI() {}
	};
}