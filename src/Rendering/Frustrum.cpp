#include "Frustrum.h"

#include "ECS.h"

using namespace Graphyte;

Frustum::Frustum(const Matrix4& v, const Matrix4& p)
{
	Matrix4 clip;

	clip[0][0] = v[0][0] * p[0][0] + v[0][1] * p[1][0] + v[0][2] * p[2][0] + v[0][3] * p[3][0];
	clip[1][0] = v[0][0] * p[0][1] + v[0][1] * p[1][1] + v[0][2] * p[2][1] + v[0][3] * p[3][1];
	clip[2][0] = v[0][0] * p[0][2] + v[0][1] * p[1][2] + v[0][2] * p[2][2] + v[0][3] * p[3][2];
	clip[3][0] = v[0][0] * p[0][3] + v[0][1] * p[1][3] + v[0][2] * p[2][3] + v[0][3] * p[3][3];
	clip[0][1] = v[1][0] * p[0][0] + v[1][1] * p[1][0] + v[1][2] * p[2][0] + v[1][3] * p[3][0];
	clip[1][1] = v[1][0] * p[0][1] + v[1][1] * p[1][1] + v[1][2] * p[2][1] + v[1][3] * p[3][1];
	clip[2][1] = v[1][0] * p[0][2] + v[1][1] * p[1][2] + v[1][2] * p[2][2] + v[1][3] * p[3][2];
	clip[3][1] = v[1][0] * p[0][3] + v[1][1] * p[1][3] + v[1][2] * p[2][3] + v[1][3] * p[3][3];
	clip[0][2] = v[2][0] * p[0][0] + v[2][1] * p[1][0] + v[2][2] * p[2][0] + v[2][3] * p[3][0];
	clip[1][2] = v[2][0] * p[0][1] + v[2][1] * p[1][1] + v[2][2] * p[2][1] + v[2][3] * p[3][1];
	clip[2][2] = v[2][0] * p[0][2] + v[2][1] * p[1][2] + v[2][2] * p[2][2] + v[2][3] * p[3][2];
	clip[3][2] = v[2][0] * p[0][3] + v[2][1] * p[1][3] + v[2][2] * p[2][3] + v[2][3] * p[3][3];
	clip[0][3] = v[3][0] * p[0][0] + v[3][1] * p[1][0] + v[3][2] * p[2][0] + v[3][3] * p[3][0];
	clip[1][3] = v[3][0] * p[0][1] + v[3][1] * p[1][1] + v[3][2] * p[2][1] + v[3][3] * p[3][1];
	clip[2][3] = v[3][0] * p[0][2] + v[3][1] * p[1][2] + v[3][2] * p[2][2] + v[3][3] * p[3][2];
	clip[3][3] = v[3][0] * p[0][3] + v[3][1] * p[1][3] + v[3][2] * p[2][3] + v[3][3] * p[3][3];

	planes[PLANE_RIGHT].x = clip[3][0] - clip[0][0];
	planes[PLANE_RIGHT].y = clip[3][1] - clip[0][1];
	planes[PLANE_RIGHT].z = clip[3][2] - clip[0][2];
	planes[PLANE_RIGHT].w = clip[3][3] - clip[0][3];

	planes[PLANE_LEFT].x = clip[3][0] + clip[0][0];
	planes[PLANE_LEFT].y = clip[3][1] + clip[0][1];
	planes[PLANE_LEFT].z = clip[3][2] + clip[0][2];
	planes[PLANE_LEFT].w = clip[3][3] + clip[0][3];

	planes[PLANE_BOTTOM].x = clip[3][0] + clip[1][0];
	planes[PLANE_BOTTOM].y = clip[3][1] + clip[1][1];
	planes[PLANE_BOTTOM].z = clip[3][2] + clip[1][2];
	planes[PLANE_BOTTOM].w = clip[3][3] + clip[1][3];

	planes[PLANE_TOP].x = clip[3][0] - clip[1][0];
	planes[PLANE_TOP].y = clip[3][1] - clip[1][1];
	planes[PLANE_TOP].z = clip[3][2] - clip[1][2];
	planes[PLANE_TOP].w = clip[3][3] - clip[1][3];

	planes[PLANE_FAR].x = clip[3][0] - clip[2][0];
	planes[PLANE_FAR].y = clip[3][1] - clip[2][1];
	planes[PLANE_FAR].z = clip[3][2] - clip[2][2];
	planes[PLANE_FAR].w = clip[3][3] - clip[2][3];

	planes[PLANE_NEAR].x = clip[3][0] + clip[2][0];
	planes[PLANE_NEAR].y = clip[3][1] + clip[2][1];
	planes[PLANE_NEAR].z = clip[3][2] + clip[2][2];
	planes[PLANE_NEAR].w = clip[3][3] + clip[2][3];

	for (int i = 0; i < 6; i++)
	{
		planes[i] = glm::normalize(planes[i]);
	}
}

bool Frustum::TestIntersection(const Transform& transform)
{
	// Is inside the frustrum
	bool result = true;

	for (int i = 0; i < 6; i++)
	{
		const float pos = planes[i].w;
		const Vector3 normal = Vector3(planes[i]);

		if (glm::dot(normal, transform.boundingBox.getPositiveVertex(normal) + transform.position) + pos < 0.0f)
		{
			return false;
		}

		if (glm::dot(normal, transform.boundingBox.getNegativeVertex(normal) + transform.position) + pos < 0.0f)
		{
			result = true;
		}
	}

	return result;
}
