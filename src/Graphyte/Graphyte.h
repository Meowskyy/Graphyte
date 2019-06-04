#pragma once
#include "glad\glad.h"
#include "GLFW\glfw3.h"

#include "Objects\Scene.h"

#include <vector>
namespace Graphyte {
	class GraphyteEditor {
	private:
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