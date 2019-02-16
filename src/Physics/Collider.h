#pragma once

#include "ECS.h"

class Bounds {
public:
	Vector3 center;
	Vector3 max;
	Vector3 min;
};

class Collider : public Component {
public:
	Bounds bounds;

	Collider() 
	{
		
	}

	// Useful for spheres
	Vector3 ClosestPoint(const Vector3& position) {}
};