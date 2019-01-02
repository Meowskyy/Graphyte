#pragma once

#include "glm\glm.hpp"

#include "GLFW\glfw3.h"

class Screen {
private:
	static int screenWidth, screenHeight;
	static int resX, resY;
public:
	static glm::vec2 GetScreenResolution();
	static void UpdateScreenResolution();

	static glm::vec2 GetWindowSize();
	static void UpdateWindowSize(GLFWwindow * window);
};