#pragma once

#include "BoundingBox.h"

class ExtraRenderer {
public:
	static void DrawAABB(BoundingBox& box, Vector3& position);
};