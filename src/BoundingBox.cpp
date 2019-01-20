#include "BoundingBox.h"

#include <cmath>

#include "Physics\Collider.h"

BoundingBox::BoundingBox(const Vector3& min, const Vector3& max)
{
	this->min = min;
	this->max = max;
}

void BoundingBox::Recalculate()
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

bool BoundingBox::Contains(const BoundingBox& otherBoundingBox)
{
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

bool BoundingBox::Contains(const Transform& transform)
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

bool BoundingBox::Touching(const Transform& transform)
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

bool BoundingBox::Touching(const Collider& collider)
{
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

	return true;
}

bool BoundingBox::TestAABBOverlap(const BoundingBox& a, const BoundingBox& b)
{
	float d1x = b.min.x - a.max.x;
	float d1y = b.min.y - a.max.y;
	float d1z = b.min.z - a.max.z;

	float d2x = a.min.x - b.max.x;
	float d2y = a.min.y - b.max.y;
	float d2z = a.min.z - b.max.z;

	if (d1x > 0.0f || d1y > 0.0f || d1z > 0.0f)
		return false;

	if (d2x > 0.0f || d2y > 0.0f || d2z > 0.0f)
		return false;

	return true;
}

bool BoundingBox::TestAABBOverlap(const Transform& a, const Transform& b)
{
	Vector3 aMin = a.boundingBox.min + a.position;
	Vector3 aMax = a.boundingBox.max + a.position;

	Vector3 bMin = b.boundingBox.min + b.position;
	Vector3 bMax = b.boundingBox.max + b.position;

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

