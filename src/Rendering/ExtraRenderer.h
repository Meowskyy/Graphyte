#pragma once

#include "Physics\BoundingBox.h"

namespace Graphyte {
	class Transform;

	class ExtraRenderer {
	public:
		static void DrawAABB(const AxisAlignedBoundingBox& box);
		static void DrawBounds(const Bounds& box);
		static void DrawLine(const Vector3& start, const Vector3& end, const Vector3& Color = Vector3(1, 1, 1));
		static void DrawRay(const Vector3& start, const Vector3& direction);
		static void DrawSelectionArrows(const Transform* transf, const float distance = 1);
	};
}