#pragma once
#include "glad\glad.h"
#include "GLFW\glfw3.h"

#include "Objects\Scene.h"

#include <vector>
namespace Graphyte {
	class GraphyteEditor {
	private:
		static void UpdateProjectionMatrix();

		static void window_size_callback(GLFWwindow* window, int width, int height);
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		static void glfw_error_callback(int error, const char* description);

		void initWindow();
		void setupCallbacks();
		void loadShaders();
		void loadTextures();
		void loadModels();
		void loadMaterials();
		void mainLoop();
		void cleanup();

		// EDITOR UI
		void DrawEditorUI();

		// IMGUI
		void SetupIMGUI();

		Camera* editorCamera;
	public:
		static GameObject* selectedGameObject;
		static Scene currentScene;
		static GLFWwindow *mainWindow;

		GraphyteEditor() = default;

		void run();
	};
}