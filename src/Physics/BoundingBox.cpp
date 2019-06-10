#include "BoundingBox.h"

#include <cmath>

#include "Physics\Collider.h"

#include <iostream>

using namespace Graphyte;

AxisAlignedBoundingBox::AxisAlignedBoundingBox(const Vector3& min, const Vector3& max)
{
	this->bounds.min = min;
	this->bounds.max = max;

	this->min = min;
	this->max = max;

	this->size = max - min;

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

void Graphyte::AxisAlignedBoundingBox::SetSize(const Vector3 min, const Vector3 max)
{
	this->bounds.min = min;
	this->bounds.max = max;

	this->min = min;
	this->max = max;

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
	for (int i = 0; i < 8; i++) {
		Vector3 transformed = transform->GetRotScaleMatrix() * Vector4(corners[i], 1.0);
		max = glm::max(max, transformed);
		min = glm::min(min, transformed);
	}

	this->min = min;
	this->max = max;
	this->center = Vector3((min.x + max.x) / 2, (min.y + max.y) / 2, (min.z + max.z) / 2);
	this->size = max - min;
}

Bounds AxisAlignedBoundingBox::GetBounds()
{
	return Bounds(this->min, this->max);
}

bool AxisAlignedBoundingBox::Contains(const AxisAlignedBoundingBox& otherBoundingBox) const
{
	if (this->min.x < otherBoundingBox.min.x + otherBoundingBox.transform->position.x &&
		this->min.y < otherBoundingBox.min.y + otherBoundingBox.transform->position.y &&
		this->min.z < otherBoundingBox.min.z + otherBoundingBox.transform->position.z &&
		this->max.x > otherBoundingBox.max.x + otherBoundingBox.transform->position.x &&
		this->max.y > otherBoundingBox.max.y + otherBoundingBox.transform->position.y &&
		this->max.z > otherBoundingBox.max.z + otherBoundingBox.transform->position.z
		)
	{
		return true;
	}

	return false;
}

bool AxisAlignedBoundingBox::Touching(const Transform& transform) const
{
	Vector3 aMin = min;
	Vector3 aMax = max;

	Vector3 bMin = transform.boundingBox.min + transform.position;
	Vector3 bMax = transform.boundingBox.max + transform.position;

	float d1x = bMin.x - aMax.x;
	float d1y = bMin.y - aMax.y;
	float d1z = bMin.z - aMax.z;

	float d2x = aMin.x - bMax.x;
	float d2y = aMin.y - bMax.y;
	float d2z = aMin.z - bMax.z;

	if (d1x > 0.0f || d1y > 0.0f || d1z > 0.0f)
		return false;

	if (d2x > 0.0f || d2y > 0.0f || d2z > 0.0f)
		return false;

	return true;
}

bool AxisAlignedBoundingBox::Touching(const Collider& collider) const
{
	/*
	Vector3 aMin = min;
	Vector3 aMax = max;

	Vector3 bMin = collider.bounds.min + collider.transform->position;
	Vector3 bMax = collider.bounds.max + collider.transform->position;

	float d1x = bMin.x - aMax.x;
	float d1y = bMin.y - aMax.y;
	float d1z = bMin.z - aMax.z;

	float d2x = aMin.x - bMax.x;
	float d2y = aMin.y - bMax.y;
	float d2z = aMin.z - bMax.z;

	if (d1x > 0.0f || d1y > 0.0f || d1z > 0.0f)
		return false;

	if (d2x > 0.0f || d2y > 0.0f || d2z > 0.0f)
		return false;
		*/

	return true;
}

bool AxisAlignedBoundingBox::TestOverlap(const AxisAlignedBoundingBox& a, const AxisAlignedBoundingBox& b)
{
	Vector3 aMin = a.min + a.transform->position;
	Vector3 aMax = a.max + a.transform->position;

	Vector3 bMin = b.min + b.transform->position;
	Vector3 bMax = b.max + b.transform->position;

	float d1x = bMin.x - aMax.x;
	float d1y = bMin.y - aMax.y;
	float d1z = bMin.z - aMax.z;

	float d2x = aMin.x - bMax.x;
	float d2y = aMin.y - bMax.y;
	float d2z = aMin.z - bMax.z;

	if (d1x > 0.0f || d1y > 0.0f || d1z > 0.0f)
		return false;

	if (d2x > 0.0f || d2y > 0.0f || d2z > 0.0f)
		return false;

	return true;
}

Vector3 AxisAlignedBoundingBox::getPositiveVertex(const Vector3& normal) const
{
	Vector3 positiveVertex = bounds.min;

	//if (normal.x >= 0.0f) { positiveVertex.x = max.x; }
	//if (normal.y >= 0.0f) { positiveVertex.y = max.y; }
	//if (normal.z >= 0.0f) { positiveVertex.z = max.z; }

	if (normal.x >= 0.0f) { positiveVertex.x = bounds.max.x; }
	if (normal.y >= 0.0f) { positiveVertex.y = bounds.max.y; }
	if (normal.z >= 0.0f) { positiveVertex.z = bounds.max.z; }

	return positiveVertex;
}

Vector3 AxisAlignedBoundingBox::getNegativeVertex(const Vector3& normal) const
{
	Vector3 negativeVertex = bounds.max;

	//if (normal.x >= 0.0f) negativeVertex.x = min.x;
	//if (normal.y >= 0.0f) negativeVertex.y = min.y;
	//if (normal.z >= 0.0f) negativeVertex.z = min.z;

	if (normal.x >= 0.0f) { negativeVertex.x = bounds.min.x; }
	if (normal.y >= 0.0f) { negativeVertex.y = bounds.min.y; }
	if (normal.z >= 0.0f) { negativeVertex.z = bounds.min.z; }

	return negativeVertex;
}

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
	if (this->min.x < transform.position.x + transform.boundingBox.min.x &&
		this->min.y < transform.position.y + transform.boundingBox.min.y &&
		this->min.z < transform.position.z + transform.boundingBox.min.z &&
		this->max.x > transform.position.x + transform.boundingBox.max.x &&
		this->max.y > transform.position.y + transform.boundingBox.max.y &&
		this->max.z > transform.position.z + transform.boundingBox.max.z
		)
	{
		return true;
	}

	return false;
}
