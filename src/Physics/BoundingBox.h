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

	Vector3 BoundingBox::getPositiveVertex(const Vector3& normal) const
	{
		Vector3 positiveVertex = min;

		if (normal.x >= 0.0f) positiveVertex.x = max.x;
		if (normal.y >= 0.0f) positiveVertex.y = max.y;
		if (normal.z >= 0.0f) positiveVertex.z = max.z;

		return positiveVertex;
	}

	Vector3 BoundingBox::getNegativeVertex(const Vector3& normal) const
	{
		Vector3 negativeVertex = max;

		if (normal.x >= 0.0f) negativeVertex.x = min.x;
		if (normal.y >= 0.0f) negativeVertex.y = min.y;
		if (normal.z >= 0.0f) negativeVertex.z = min.z;

		return negativeVertex;
	}
};