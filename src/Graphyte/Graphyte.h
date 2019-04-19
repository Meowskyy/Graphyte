#pragma once
#include "glad\glad.h"
#include "GLFW\glfw3.h"

#include "Objects\Scene.h"

#include <vector>

class Graphyte {
private:
	void initWindow();
	void setupCallbacks();
	void loadShaders();
	void loadModels();
	void mainLoop();
	void cleanup();

	// UI
	void DrawUI();

	// IMGUI
	void SetupIMGUI();
public:
	static GameObject* selectedGameObject;
	static Scene currentScene;
	static GLFWwindow *mainWindow;

	Graphyte() = default;

	void run();
};