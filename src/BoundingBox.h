#pragma once
#include "glm\vec3.hpp"

#include <algorithm>
#include <cmath>

struct BoundingBox
{
public:
	glm::vec3 min;
	glm::vec3 max;

	BoundingBox() {

	}

	BoundingBox(glm::vec3& min, glm::vec3& max) {
		this->min = min;
		this->max = max;
	}

	/// <summary>
	/// This finds the smallest enclosing cube with power of 2 dimensions for the given bounding box
	/// </summary>
	/// <param name="region">The bounding box to cubify</param>
	/// <returns>A cubified version of the input parameter.</returns>
	static BoundingBox* FindEnclosingCube(BoundingBox region)
	{
		//we can't guarantee that all bounding regions will be relative to the origin, so to keep the math
		//simple, we're going to translate the existing region to be oriented off the origin and remember the translation.
		//find the min offset from (0,0,0) and translate by it for a short while
		glm::vec3 offset = glm::vec3(0, 0, 0) - region.min;
		region.min += offset;
		region.max += offset;

		//A 3D rectangle has a length, height, and width. Of those three dimensions, we want to find the largest dimension.
		//the largest dimension will be the minimum dimensions of the cube we're creating.
		int highX = (int)std::ceil(std::max(std::max(region.max.x, region.max.y), region.max.z));

		//see if our cube dimension is already at a power of 2. If it is, we don't have to do any work.
		for (int bit = 0; bit < 32; bit++)
		{
			if (highX == 1 << bit)
			{
				region.max = glm::vec3(highX, highX, highX);

				region.min -= offset;
				region.max -= offset;
				return new BoundingBox(region.min, region.max);
			}
		}

		//We have a cube with non-power of two dimensions. We want to find the next highest power of two.
		//example: 63 -> 64; 65 -> 128;
		int x = pow2roundup(highX);

		region.max = glm::vec3(x, x, x);

		region.min -= offset;
		region.max -= offset;

		return new BoundingBox(region.min, region.max);
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

	// TODO: This is supposed to check if the other bounding box fits inside completely
	// And not if its just touching it
	bool Contains(BoundingBox& otherBoundingBox) {
		if (this->min.x < otherBoundingBox.min.x &&
			this->min.y < otherBoundingBox.min.y &&
			this->min.z < otherBoundingBox.min.z &&
			this->max.x > otherBoundingBox.max.x &&
			this->max.y > otherBoundingBox.max.y &&
			this->max.z > otherBoundingBox.max.z
			) 
			{
				return true;
			}

		return false;
	}

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