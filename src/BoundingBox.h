#pragma once

#include "Math\Math.h"

class Transform;
class Collider;

struct BoundingBox
{
public:
	Vector3 min;
	Vector3 max;
	Vector3 center;
	Vector3 size;

	BoundingBox() {}
	BoundingBox(const Vector3& min, const Vector3& max);

	void Recalculate();

	bool Contains(const BoundingBox& otherBoundingBox);
	bool Contains(const Transform& transform);

	// Is this touching the other Collider
	bool Touching(const Transform& transform);
	bool Touching(const Collider& transform);

	// Tests for bounding box overlap
	static bool TestAABBOverlap(const BoundingBox& a, const BoundingBox& b);

	// Tests for bounding box overlap with Transforms
	static bool TestAABBOverlap(const Transform& a, const Transform& b);
};