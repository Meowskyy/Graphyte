#pragma once

#include "Math\Math.h"

#include <vector>

namespace Graphyte {
	class Transform;
	class Collider;

	class Bounds {
	public:
		Vector3 center;
		Vector3 max;
		Vector3 min;

		Bounds() = default;

		Bounds(const Vector3& min, const Vector3 max) 
		{
			this->min = min;
			this->max = max;

			this->center = Vector3((min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2);
		}

		// Tests for bounds overlap
		bool Contains(const Vector3 position) const;
		bool Contains(const Bounds bounds) const;
		bool Contains(const Transform& transform) const;
	};

	struct AxisAlignedBoundingBox
	{
	private:
		float maxFloat = std::numeric_limits<float>::max();
		float minFloat = std::numeric_limits<float>::min();

		std::vector<Vector3> corners = std::vector<Vector3>(8);
	public:
		Transform* transform;

		Bounds bounds;

		Vector3 center;		// Center of the AABB
		Vector3 halfSize;	// Halfsize of the AABB

		AxisAlignedBoundingBox() = default;
		AxisAlignedBoundingBox(Transform* trans) : transform(trans) {};
		AxisAlignedBoundingBox(const Vector3& min, const Vector3& max);

		void SetSize(const Vector3 min, const Vector3 max);

		void Recalculate();

		Bounds GetBounds() const;

		bool Contains(const AxisAlignedBoundingBox& otherBoundingBox) const;

		Vector3 GetPositiveVertex(const Vector3& normal) const;
		Vector3 GetNegativeVertex(const Vector3& normal) const;

		// Tests for AABB overlap
		static bool TestOverlap(const AxisAlignedBoundingBox& a, const AxisAlignedBoundingBox& b);
		static bool TestOverlapExp(const AxisAlignedBoundingBox & a, const AxisAlignedBoundingBox & b);
	};
}