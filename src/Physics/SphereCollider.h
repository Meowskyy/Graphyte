#pragma once

#include "Collider.h"

class SphereCollider : public Collider
{
public:
	// Useful for spheres
	Vector3 ClosestPoint(const Vector3& position) {}
};