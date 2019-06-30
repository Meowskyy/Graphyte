#pragma once
#include "glad\glad.h"
#include "GLFW\glfw3.h"

#include "Objects\Scene.h"

#include <vector>
namespace Graphyte {
	class GraphyteEditor {
	private:
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

		bool physicsEnabled = true;
		bool show_demo_window = true;
		bool show_transform_window = true;
		bool show_camera_window = true;
		bool show_scene_window = true;
		bool backFaceCulling = false;
		bool wireframe = false;
		bool alpha = false;

		static void UpdateProjectionMatrix();

		static void window_size_callback(GLFWwindow* window, int width, int height);
		static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
		static void glfw_error_callback(int error, const char* description);

		void InitWindow();
		void SetupCallbacks();
		void MainLoop();
		void Cleanup();

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