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

	BoundingBox() {}

	BoundingBox(Vector3& min, Vector3& max) {
		this->min = min;
		this->max = max;
	}

	static int pow2roundup(int x)
	{
		if (x < 0)
			return 0;
		--x;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		return x++;

	}

	bool Contains(BoundingBox& otherBoundingBox);
	bool Contains(Transform& transform);

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
};