#pragma once

#include "ECS.h"

namespace Graphyte {
	class Mesh;
	class Rigidbody;

	class Triangle {
	public:
		Vector3 v0, v1, v2;
		Vector3 normal;

		Triangle(Vector3 vertexA, Vector3 vertexB, Vector3 vertexC) : v0(vertexA), v1(vertexB), v2(vertexC) {};
	};

	class Plane {
	public:
		float equation[4];
		Vector3 origin;
		Vector3 normal;

		Plane() {}

		Plane(const Vector3& origin, const Vector3& normal);
		Plane(const Vector3& p1, const Vector3& p2, const Vector3& p3);
		bool isFrontFacingTo(const Vector3& direction) const;
		double signedDistanceTo(const Vector3& point) const;
	};

	class Collider : public Component {
	private:
		Rigidbody* rigidbody;

		std::vector<Vector3> points = std::vector<Vector3>();
		std::vector<unsigned int> pointIndices = std::vector<unsigned int>();

		bool TriTriCollisionDetect(const Triangle& triangle1, const Triangle& triangle2) const;
		bool PointInTriangle(const Vector3 triV1, const Vector3 triV2, const Vector3 triV3, const Vector3 point) const;
	public:
		std::vector<Plane> planes = std::vector<Plane>();

		Mesh* mesh;
		bool isConvex = true;
		bool showInEditor = false;

		void OnComponentAdded();

		bool hasCollision = true;

		// Useful for spheres
		Vector3 ClosestPoint(const Vector3& position) const;

		bool SphereTriCollisionDetect(const Vector3 position, const Plane & plane);
		bool CheckForTriangleIntersection(const Triangle& triangleA, const Triangle& triangleB) const;
		bool CheckForPlaneIntersection(const Plane& pn1, const Plane& pn2) const;
		
		bool CheckCollision(Collider& col);

		void CreateConvex();
		void SetMesh(Mesh& mesh);

		void DrawCollider();

		void DrawUI() {
			ImGui::Checkbox("isConcave?", &isConvex);
			ImGui::Checkbox("ShowInEditor", &showInEditor);
		}
	};
}