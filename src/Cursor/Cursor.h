#pragma once

#include "GLFW\glfw3.h"
#include "Graphyte\Graphyte.h"

class MouseCursor {
public:
	static void SetVisible(bool isVisible) 
	{	
		if (isVisible)
		{
			//glfwSetInputMode(Graphyte::mainWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			//glfwSetInputMode(Graphyte::mainWindow, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}

		//std::cout << "Mouse Visible: " << isVisible << std::endl;
	}
};