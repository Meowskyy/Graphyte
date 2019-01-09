#include "BoundingBox.h"

#include "Objects\Transform.h"

#include <cmath>

bool BoundingBox::Contains(BoundingBox & otherBoundingBox)
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

bool BoundingBox::Contains(Transform& transform)
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

bool BoundingBox::TestAABBOverlap(Transform* a, Transform* b)
{
	Vector3 aMin = a->boundingBox.min + a->position;
	Vector3 aMax = a->boundingBox.max + a->position;

	Vector3 bMin = b->boundingBox.min + b->position;
	Vector3 bMax = b->boundingBox.max + b->position;

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
