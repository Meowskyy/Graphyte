#pragma once

#include "BoundingBox.h"

class ExtraRenderer {
public:
	static void DrawAABB(const BoundingBox& box, const Vector3& position);
	static void DrawUniformBox(const BoundingBox& box);
};