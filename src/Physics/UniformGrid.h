#pragma once

#include "ECS.h"

#include "BoundingBox.h"

#include "Rendering\ExtraRenderer.h"

#include <vector>
#include <queue>

#include <map>

const int minSize = 8;
const int maxSize = 65536;

namespace Graphyte {
	class UniformGrid {
	private:
		static std::map<int, std::vector<int>> currentlyCollidingObjects;

		float size;

		bool activeChildren[8];
		int activeChildCount = 0;
		std::vector<UniformGrid> childGrid;

		static std::map<GameObject*, std::vector<Collider*>> PendingColliders;

		//static std::queue<Collider*> pendingColliders;			// Colliders that will be added later
		static std::queue<int> pendingColliders;			// Colliders that will be added later
		static std::vector<Collider*> allColliders;

		//std::vector<Collider*> colliders;
		std::vector<int> colliders;

		UniformGrid(const Bounds& boundaries);					// Create UniformGrid with boundaries
		UniformGrid CreateNode(const Bounds& boundary); 		// Create child with boundaries

	public:
		static bool gridReady; // Is the grid ready to accept colliders?

		UniformGrid* parent;

		Bounds boundaries;

		// CONSTRUCTORS
		UniformGrid();					// Default
		UniformGrid(const int size); 	// With size

		bool isRoot() const;

		static void AddCollider(Collider* col);

		bool InsertCollider(const int colliderPosition);

		void RebuildGrid();
		void Update();

		void CheckCollisions();
		void CollidingObjects(const int colliderIndex);

		//std::vector<Renderer*> GetObjectsToRender(Camera& camera);

		bool IsInsideWorldBounds(const Transform& transform) const;

		void DrawGrid() const;
		void DrawExtra() const;
	};
}