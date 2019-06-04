#pragma once

#include "ECS.h"

class Mesh;

namespace Graphyte {
	class Bounds {
	public:
		Vector3 center;
		Vector3 max;
		Vector3 min;
	};

	class Collider : public Component {
	private:
		std::vector<Vector3> points;
		std::vector<unsigned int> pointIndices;

	public:
		Mesh *mesh;
		Bounds bounds;
		bool isConvex = true;
		bool showInEditor = false;

		Collider()
		{

		}

		void OnComponentAdded();

		// Useful for spheres
		Vector3 ClosestPoint(const Vector3& position);

		void CreateConvex();

		void Update() {
			if (showInEditor)
			{
				DrawCollider();
			}
		}
		void DrawCollider();

		void DrawUI() {
			ImGui::Checkbox("isConcave?", &isConvex);
			ImGui::Checkbox("ShowInEditor", &showInEditor);
		}
	};
}