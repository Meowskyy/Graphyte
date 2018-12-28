#include "Graphyte.h"

#include <iostream>

// STANDARD
#include "Cursor.h"
#include "Input.h"
#include "Screen.h"

#include "Camera.h"

#include "TextRenderer.h"
#include "ResourceManager.h"
#include "MeshRenderer.h"

#include "ModelLoader.h"

// SFML AUDIO
#include "AudioListener.h"
#include "AudioEmitter.h"

// CUSTOM
#include "MoveAround.h"
#include "CameraOrbit.h"

#include "imgui.h"

#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

Scene Graphyte::currentScene;

GLFWwindow* Graphyte::mainWindow;

GameObject* Graphyte::selectedGameObject;

// timing
float lastFrame = 0.0f;

enum test_enum {
	Item1 = 0,
	Item2,
	Item3
};

void Graphyte::run() 
{
	initWindow();

	loadShaders();

	mainLoop();
	cleanup();
}

static void glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

//Model test;
void Graphyte::initWindow()
{
	// Setup window
	glfwSetErrorCallback(glfw_error_callback);

	if (!glfwInit()) 
	{
		throw std::runtime_error("Could not initialize GLFW!");
	}

	glfwSetTime(0);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	glfwWindowHint(GLFW_SAMPLES, 0);
	glfwWindowHint(GLFW_RED_BITS, 8);
	glfwWindowHint(GLFW_GREEN_BITS, 8);
	glfwWindowHint(GLFW_BLUE_BITS, 8);
	glfwWindowHint(GLFW_ALPHA_BITS, 8);
	glfwWindowHint(GLFW_STENCIL_BITS, 8);
	glfwWindowHint(GLFW_DEPTH_BITS, 24);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);

	// Create a GLFWwindow object
	mainWindow = glfwCreateWindow(1920, 1080, "Graphyte", NULL, NULL);
	if (mainWindow == nullptr) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(mainWindow);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) 
	{
		throw std::runtime_error("Could not initialize GLAD!");
	}

	int width, height;
	glfwGetFramebufferSize(mainWindow, &width, &height);
	glViewport(0, 0, width, height);
	glfwSwapInterval(0);

	SetupIMGUI();

	loadShaders();
	loadModels();

	currentScene.AddCameraObject();

	setupCallbacks();
}

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
void Graphyte::mainLoop() 
{
	float currentTime = glfwGetTime();

	while (!glfwWindowShouldClose(mainWindow)) {
		Input::UpdateKeyStates();
		Input::UpdateMousePosition();

		glfwPollEvents();

		// IMGUI
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//ImGui_ImplGlfwGL3_NewFrame();

		//glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // also clear the depth buffer now!

		DrawUI();

		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(mainWindow);
		glfwGetFramebufferSize(mainWindow, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Update GameObjects
		//currentScene.FixedUpdate();
		currentScene.Update();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwMakeContextCurrent(mainWindow);
		glfwSwapBuffers(mainWindow);
	}
}

bool show_demo_window = true;
bool show_transform_window = true;
bool show_camera_window = true;
bool show_scene_window = true;
void Graphyte::DrawUI()
{
	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (show_demo_window) 
	{
		ImGui::ShowDemoWindow(&show_demo_window);
	}

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Debug Window");

		//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		/*
		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);
		*/

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_transform_window)
	{
		ImGui::Begin("Transform", &show_transform_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

		if (selectedGameObject) {
			//ImGui::Text("Name: %s", selectedGameObject->transform.name.c_str());
			char *arr = &selectedGameObject->transform.name[0u];

			//std::cout << IM_ARRAYSIZE(arr) << std::endl;
			//std::cout << sizeof(selectedGameObject->transform.name) << std::endl;

			// TODO: Fixing a nasty bug that causes the program to crash whenever the length is 24 or more characters?
			ImGui::InputText("Name", arr, IM_ARRAYSIZE(arr) * 2);

			ImGui::DragFloat3("Position", (float*)&selectedGameObject->transform.position);
			ImGui::DragFloat3("Rotation", (float*)&selectedGameObject->transform.rotation);
			ImGui::DragFloat3("Scale", (float*)&selectedGameObject->transform.scale);

			static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
			int node_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
			ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3); // Increase spacing to differentiate leaves from expanded contents.
			for (int i = 0; i < selectedGameObject->behaviour.size(); i++)
			{
				// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
				ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);
				// Node
				bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, selectedGameObject->behaviour[i]->name().c_str(), i);
				if (ImGui::IsItemClicked()) {
					//selectedGameObject = gameObjects[i];
					node_clicked = i;
				}
				if (node_open)
				{
					ImGui::Text("Add behaviour information here, like variables");
					ImGui::TreePop();
				}
			}
			if (node_clicked != -1)
			{
				// Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
				if (ImGui::GetIO().KeyCtrl)
					selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
				else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
					selection_mask = (1 << node_clicked);           // Click to single-select
			}
			ImGui::PopStyleVar();


			if (ImGui::Button("Add Behaviour"))
			{

			}

			// TODO: Making this dynamic instead
			if (ImGui::Button("Add AudioEmitter"))
			{
				selectedGameObject->AddBehaviour(new AudioEmitter(selectedGameObject->transform, "Assets/resources/Audio/testAudio.wav"));
			}

			if (ImGui::Button("Add MoveAround"))
			{
				selectedGameObject->AddBehaviour(new MoveAround(selectedGameObject->transform));
			}
		}

		ImGui::End();
	}

	if (show_scene_window) 
	{
		ImGui::Begin("Scene");
		if (ImGui::Button("Create GameObject"))
			currentScene.AddGameObject();

		if (ImGui::Button("Create Child GameObject"))
			currentScene.AddChild();

		if (ImGui::Button("Create 1000 GameObjects"))
			currentScene.Add1000GameObjects();

		static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
		int node_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
		ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3); // Increase spacing to differentiate leaves from expanded contents.
		for (int i = 0; i < currentScene.gameObjects.size(); i++)
		{
			// Disable the default open on single-click behavior and pass in Selected flag according to our selection state.
			ImGuiTreeNodeFlags node_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ((selection_mask & (1 << i)) ? ImGuiTreeNodeFlags_Selected : 0);
			// Node
			bool node_open = ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, currentScene.gameObjects[i]->transform.name.data(), i);
			if (ImGui::IsItemClicked()) {
				selectedGameObject = currentScene.gameObjects[i];
				node_clicked = i;
			}
			if (node_open)
			{
				currentScene.gameObjects[i]->transform.DrawUI();
				ImGui::TreePop();
			}
		}
		if (node_clicked != -1)
		{
			// Update selection state. Process outside of tree loop to avoid visual inconsistencies during the clicking-frame.
			if (ImGui::GetIO().KeyCtrl)
				selection_mask ^= (1 << node_clicked);          // CTRL+click to toggle
			else //if (!(selection_mask & (1 << node_clicked))) // Depending on selection behavior you want, this commented bit preserve selection when clicking on item that is part of the selection
				selection_mask = (1 << node_clicked);           // Click to single-select
		}
		ImGui::PopStyleVar();

		ImGui::End();
	}

}

void Graphyte::SetupIMGUI() 
{
	const char* glsl_version = "#version 330";
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
	io.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;   // Enable Gamepad Controls

	ImGui_ImplGlfw_InitForOpenGL(mainWindow, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'misc/fonts/README.txt' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);
}

void Graphyte::cleanup() 
{
	// Cleanup
	//ImGui_ImplGlfwGL3_Shutdown();

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

// TODO: Moving these somewhere more appropriate
void Graphyte::loadShaders() 
{
	ResourceManager::LoadShader("Shaders/Standard.vert", "Shaders/Standard.frag", nullptr, "Standard");
}

// TODO: Moving these somewhere more appropriate
void Graphyte::loadModels()
{
	//test = Model("Assets/resources/nanosuit/nanosuit.obj");

	//TextRenderer::Init();
	//TextRenderer::SetShader(ResourceManager::GetShader("GUIText"));
}

void Graphyte::setupCallbacks()
{
	// SET CALLBACKS
	glfwSetCursorPosCallback(mainWindow, Input::mouse_callback);
	glfwSetMouseButtonCallback(mainWindow, Input::mouse_button_callback);
	glfwSetKeyCallback(mainWindow, Input::key_callback);
}
