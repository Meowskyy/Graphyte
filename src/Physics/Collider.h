#pragma once

#include "ECS.h"

class Mesh;

namespace Graphyte {
	class Triangle {
		Vector3 v1, v2, v3;
	};

	class Collider : public Component {
	private:
		std::vector<Vector3> points = std::vector<Vector3>();
		std::vector<unsigned int> pointIndices = std::vector<unsigned int>();

	public:
		Mesh* mesh;
		bool isConvex = true;
		bool showInEditor = false;

		void OnComponentAdded();

		bool hasCollision = true;

		// Useful for spheres
		Vector3 ClosestPoint(const Vector3& position);
		bool CheckForTriangleIntersection(const Triangle& triangleA, const Triangle& triangleB);

		void CreateConvex();

		void DrawCollider();

		void DrawUI() {
			ImGui::Checkbox("isConcave?", &isConvex);
			ImGui::Checkbox("ShowInEditor", &showInEditor);
		}
	};
}