#include "Screen.h"

#include <iostream>

using namespace Graphyte;

int Screen::width, Screen::height;
Vector2 Screen::resolution;

Vector2 Screen::GetScreenResolution()
{
	return Vector2(0, 0);
}

void Screen::UpdateScreenResolution()
{
	const GLFWvidmode* mode = glfwGetVideoMode(glfwGetPrimaryMonitor());

	//resX = mode->width;
	//resY = mode->height;
	//std::cout << "Screen Resolution: " << resX << ", " << resY << std::endl;
}

Vector2 Screen::GetWindowSize()
{
	return Vector2(width, height);
}

void Screen::UpdateWindowSize(int newWidth, int newHeight)
{
	width = newWidth;
	height = newHeight;

	std::cout << "Window Size: " << width << ", " << height << std::endl;
}
