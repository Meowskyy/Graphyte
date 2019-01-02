#include "Screen.h"

#include <iostream>

int Screen::resX, Screen::resY;
int Screen::screenWidth, Screen::screenHeight;

glm::vec2 Screen::GetScreenResolution()
{
	return glm::vec2(resX, resY);
}

void Screen::UpdateScreenResolution()
{
	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	resX = mode->width;
	resY = mode->height;
	std::cout << "Screen Resolution: " << resX << ", " << resY << std::endl;
}

glm::vec2 Screen::GetWindowSize()
{
	return glm::vec2(screenWidth, screenHeight);
}

void Screen::UpdateWindowSize(GLFWwindow * window)
{
	glfwGetWindowSize(window, &screenWidth, &screenHeight);

	std::cout << "Window Size: " << screenWidth << ", " << screenHeight << std::endl;
}
