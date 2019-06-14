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
typedef glm::mat4 Matrix4;

const float PI = 3.141592653589793;

namespace Graphyte {
	/*
	struct Vector2 {

	};

	struct Vector3 {

	};

	struct Quaternion {

	};
	*/

	struct Matrix4x4 {
		float m[4][4];
	};

	class Math {
	public:

		template <typename t>
		static t lerp(t x, t y, t time) {
			return x * (1.f - time) + y * time;
		}

		template <typename t>
		static t Clamp(t value, t min, t max)
		{
			if (value < min) value = min;
			if (value > max) value = max;
			return value;
		}

		template <typename t>
		static t Clamp01(t value)
		{
			return Clamp(value, 0, 1);
		}
	};
}