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
		float size;

		bool activeChildren[8];
		int activeChildCount = 0;
		std::vector<UniformGrid> childGrid;

		static std::map<GameObject*, std::vector<Collider*>> PendingColliders;
		static std::queue<Collider*> pendingColliders;			// Colliders that will be added later
		static std::vector<Collider*> allColliders;
		std::vector<Collider*> colliders;

		UniformGrid(const Bounds& boundaries);					// Create UniformGrid with boundaries
		UniformGrid CreateNode(const Bounds& boundary); 		// Create child with boundaries

	public:
		static bool gridReady; // FALSE by default. the tree has a few objects which need to be inserted before it is complete 
		static std::vector<std::string> collidingGameObjects;	// GameObjects that are touching currently

		UniformGrid* parent;

		Bounds boundaries;

		// CONSTRUCTORS
		UniformGrid();					// Default
		UniformGrid(const int size); 	// With size

		bool isRoot() const;

		static void AddCollider(Collider* col);

		bool InsertCollider(Collider& collider);

		void CollidingObjects(Collider& collider) const;
		void CollidingObjects(Collider& collider, const int index) const;

		bool IsInsideBounds(const Transform& transform) const;

		void RebuildGrid();
		void Update();

		void DrawGrid() const;
		void DrawExtra() const;
	};
}