#pragma once

#include "glm\glm.hpp"

#include "GLFW\glfw3.h"

#include "Math\Math.h"

namespace Graphyte {
	class Screen {
	private:

	public:
		static int width, height;
		static Vector2 resolution;

		static Vector2 GetScreenResolution();
		static void UpdateScreenResolution();

		static Vector2 GetWindowSize();
		static void UpdateWindowSize(int width, int height);
	};
}