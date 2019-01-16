#pragma once
#include <algorithm>
#include <cmath>

#include "Math\Vector3.h"

class Transform;

struct BoundingBox
{
public:
	Vector3 min;
	Vector3 max;
	Vector3 center;
	Vector3 size;

	BoundingBox() {}

	BoundingBox(Vector3& min, Vector3& max) {
		this->min = min;
		this->max = max;

		Recalculate();
	}

	void Recalculate()
	{
		float centerX = (min.x + max.x) / 2;
		float centerY = (min.y + max.y) / 2;
		float centerZ = (min.z + max.z) / 2;
		Vector3 center = Vector3(centerX, centerY, centerZ);

		this->center = center;

		// TODO: Calculate bounding box based on rotation
		this->size.x = max.x - min.x;
		this->size.y = max.y - min.y;
		this->size.z = max.z - min.z;
	}


	bool Contains(BoundingBox& otherBoundingBox);
	bool Contains(Transform& transform);
	bool Touching(Transform& transform);

	// Tests for bounding box overlap
	static bool TestAABBOverlap(BoundingBox* a, BoundingBox* b)
	{
		float d1x = b->min.x - a->max.x;
		float d1y = b->min.y - a->max.y;
		float d1z = b->min.z - a->max.z;

		float d2x = a->min.x - b->max.x;
		float d2y = a->min.y - b->max.y;
		float d2z = a->min.z - b->max.z;

		if (d1x > 0.0f || d1y > 0.0f || d1z > 0.0f)
			return false;

		if (d2x > 0.0f || d2y > 0.0f || d2z > 0.0f)
			return false;

		return true;
	}

	// Tests for bounding box overlap with Transforms
	static bool TestAABBOverlap(Transform* a, Transform* b);
};