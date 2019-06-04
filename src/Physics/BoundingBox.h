#pragma once

#include "Math\Math.h"

namespace Graphyte {
	class Transform;
	class Collider;

	struct BoundingBox
	{
	private:

	public:
		Vector3 min;
		Vector3 max;
		Vector3 center;
		Vector3 size;
		Vector3* position;
		Transform* transform;

		BoundingBox() = default;
		BoundingBox(const Vector3& min, const Vector3& max);

		void Recalculate();

		bool Contains(const BoundingBox& otherBoundingBox) const;
		bool Contains(const Transform& transform) const;

		// Is this touching the other Collider
		bool Touching(const Transform& transform) const;
		bool Touching(const Collider& transform) const;

		Vector3 BoundingBox::getPositiveVertex(const Vector3& normal) const;

		Vector3 BoundingBox::getNegativeVertex(const Vector3& normal) const;

		// Tests for bounding box overlap
		static bool TestAABBOverlap(const BoundingBox& a, const BoundingBox& b);

		// Tests for bounding box overlap with Transforms
		static bool TestAABBOverlap(const Transform& a, const Transform& b);
	};
}