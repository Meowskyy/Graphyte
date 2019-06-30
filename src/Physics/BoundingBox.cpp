#include "BoundingBox.h"

#include <cmath>

#include "Physics\Collider.h"

#include <iostream>

using namespace Graphyte;

bool Bounds::Contains(const Vector3 position) const
{
	if (this->min.x < position.x &&
		this->min.y < position.y &&
		this->min.z < position.z &&
		this->max.x > position.x &&
		this->max.y > position.y &&
		this->max.z > position.z
		)
	{
		return true;
	}

	return false;
}

bool Bounds::Contains(const Bounds bounds) const
{
	if (this->min.x < bounds.min.x &&
		this->min.y < bounds.min.y &&
		this->min.z < bounds.min.z &&
		this->max.x > bounds.max.x &&
		this->max.y > bounds.max.y &&
		this->max.z > bounds.max.z
		)
	{
		return true;
	}

	return false;
}

bool Bounds::Contains(const Transform& transform) const
{
	if (this->min.x < transform.boundingBox.center.x + -transform.boundingBox.halfSize.x &&
		this->min.y < transform.boundingBox.center.y + -transform.boundingBox.halfSize.y &&
		this->min.z < transform.boundingBox.center.z + -transform.boundingBox.halfSize.z &&
		this->max.x > transform.boundingBox.center.x + transform.boundingBox.halfSize.x &&
		this->max.y > transform.boundingBox.center.y + transform.boundingBox.halfSize.y &&
		this->max.z > transform.boundingBox.center.z + transform.boundingBox.halfSize.z
		)
	{
		return true;
	}

	return false;
}

AxisAlignedBoundingBox::AxisAlignedBoundingBox(const Vector3& min, const Vector3& max)
{
	this->bounds.min = min;
	this->bounds.max = max;

	corners[0] = this->bounds.min;
	corners[1] = Vector3(this->bounds.min.x, this->bounds.min.y, this->bounds.max.z);
	corners[2] = Vector3(this->bounds.min.x, this->bounds.max.y, this->bounds.min.z);
	corners[3] = Vector3(this->bounds.max.x, this->bounds.min.y, this->bounds.min.z);
	corners[4] = Vector3(this->bounds.min.x, this->bounds.max.y, this->bounds.max.z);
	corners[5] = Vector3(this->bounds.max.x, this->bounds.min.y, this->bounds.max.z);
	corners[6] = Vector3(this->bounds.max.x, this->bounds.max.y, this->bounds.min.z);
	corners[7] = this->bounds.max;

	//bounds = GetBounds();

	//Recalculate();
}

void AxisAlignedBoundingBox::SetSize(const Vector3 min, const Vector3 max)
{
	this->bounds.min = min;
	this->bounds.max = max;

	corners[0] = this->bounds.min;
	corners[1] = Vector3(this->bounds.min.x, this->bounds.min.y, this->bounds.max.z);
	corners[2] = Vector3(this->bounds.min.x, this->bounds.max.y, this->bounds.min.z);
	corners[3] = Vector3(this->bounds.max.x, this->bounds.min.y, this->bounds.min.z);
	corners[4] = Vector3(this->bounds.min.x, this->bounds.max.y, this->bounds.max.z);
	corners[5] = Vector3(this->bounds.max.x, this->bounds.min.y, this->bounds.max.z);
	corners[6] = Vector3(this->bounds.max.x, this->bounds.max.y, this->bounds.min.z);
	corners[7] = this->bounds.max;
}

void AxisAlignedBoundingBox::Recalculate()
{
	Vector3 max = Vector3(minFloat, minFloat, minFloat);
	Vector3 min = Vector3(maxFloat, maxFloat, maxFloat);

	// Transform all of the corners, and keep track of the greatest and least
	// values we see on each coordinate axis.
	Matrix4 rotationScaleMatrix = transform->GetRotScaleMatrix();
	for (int i = 0; i < 8; i++) {
		Vector3 transformed = rotationScaleMatrix * Vector4(corners[i], 1.0);
		max = glm::max(max, transformed);
		min = glm::min(min, transformed);
	}

	this->halfSize = (max - min) * 0.5f;        // halfwidths
	this->center = max - halfSize + transform->position;
}

Bounds AxisAlignedBoundingBox::GetBounds() const
{
	return Bounds(-this->halfSize, this->halfSize);
}

bool AxisAlignedBoundingBox::Contains(const AxisAlignedBoundingBox& otherBoundingBox) const
{
	if (-this->halfSize.x < -otherBoundingBox.halfSize.x + otherBoundingBox.center.x &&
		-this->halfSize.y < -otherBoundingBox.halfSize.y + otherBoundingBox.center.y &&
		-this->halfSize.z < -otherBoundingBox.halfSize.z + otherBoundingBox.center.z &&
		this->halfSize.x > otherBoundingBox.halfSize.x + otherBoundingBox.center.x &&
		this->halfSize.y > otherBoundingBox.halfSize.y + otherBoundingBox.center.y &&
		this->halfSize.z > otherBoundingBox.halfSize.z + otherBoundingBox.center.z
		)
	{
		return true;
	}

	return false;
}

Vector3 AxisAlignedBoundingBox::GetPositiveVertex(const Vector3& normal) const
{
	Vector3 positiveVertex = -halfSize;

	if (normal.x >= 0.0f) { positiveVertex.x = halfSize.x; }
	if (normal.y >= 0.0f) { positiveVertex.y = halfSize.y; }
	if (normal.z >= 0.0f) { positiveVertex.z = halfSize.z; }

	return positiveVertex + center;
}

Vector3 AxisAlignedBoundingBox::GetNegativeVertex(const Vector3& normal) const
{
	Vector3 negativeVertex = halfSize;

	if (normal.x >= 0.0f) { negativeVertex.x = -halfSize.x; }
	if (normal.y >= 0.0f) { negativeVertex.y = -halfSize.y; }
	if (normal.z >= 0.0f) { negativeVertex.z = -halfSize.z; }

	return negativeVertex + center;
}

bool AxisAlignedBoundingBox::TestOverlap(const AxisAlignedBoundingBox& a, const AxisAlignedBoundingBox& b)
{
	// SIMD optimized AABB-AABB test
	// Optimized by removing conditional branches
	bool x = glm::abs(a.center[0] - b.center[0]) <= (a.halfSize[0] + b.halfSize[0]);
	bool y = glm::abs(a.center[1] - b.center[1]) <= (a.halfSize[1] + b.halfSize[1]);
	bool z = glm::abs(a.center[2] - b.center[2]) <= (a.halfSize[2] + b.halfSize[2]);

	return x && y && z;
}

bool AxisAlignedBoundingBox::TestOverlapExp(const AxisAlignedBoundingBox& a, const AxisAlignedBoundingBox& b)
{
	const Vector3 aMin = -a.halfSize + a.center;
	const Vector3 aMax = a.halfSize + a.center;

	const Vector3 bMin = -b.halfSize + b.center;
	const Vector3 bMax = b.halfSize + b.center;

	const float d1x = bMin.x - aMax.x;
	const float d1y = bMin.y - aMax.y;
	const float d1z = bMin.z - aMax.z;

	const float d2x = aMin.x - bMax.x;
	const float d2y = aMin.y - bMax.y;
	const float d2z = aMin.z - bMax.z;

	if (d1x >= 0.0f || d1y >= 0.0f || d1z >= 0.0f)
		return false;

	if (d2x >= 0.0f || d2y >= 0.0f || d2z >= 0.0f)
		return false;

	return true;
}

