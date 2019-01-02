#pragma once
#include "glad\glad.h"
#include "GLFW\glfw3.h"

#include "Objects\Scene.h"

#include <vector>

class Graphyte {
private:

public:
	static GameObject* selectedGameObject;
	static Scene currentScene;
	static GLFWwindow *mainWindow;

	void run();
	void cleanup();
	void loadShaders();
	void loadModels();
	void setupCallbacks();
	void mainLoop();
	void initWindow();

	// UI
	void DrawUI();

	// IMGUI
	void SetupIMGUI();
};