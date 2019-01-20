#pragma once
#include <algorithm>

#include "glm\vec4.hpp"
#include "glm\vec3.hpp"
#include "glm\vec2.hpp"
#include "glm\gtc\quaternion.hpp"

typedef glm::vec4 Vector4;
typedef glm::vec3 Vector3;
typedef glm::vec2 Vector2;
typedef glm::quat Quaternion;

template <typename t>
class Math {
	static t Clamp(t value, t min, t max)
	{
		if (value < min) value = min;
		if (value > max) value = max;
		return value;
	}

	static t Clamp01(t value)
	{
		//if (value < 0) value = min;
		//if (value > 1) value = max;
		return Clamp(value, 0, 1);
	}
};