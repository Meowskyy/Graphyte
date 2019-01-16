#include "Graphyte.h"

#include <iostream>

// STANDARD
#include "Cursor\Cursor.h"
#include "Input\Input.h"
#include "Rendering\Screen.h"
#include "Rendering\Camera.h"

#include "Text\TextRenderer.h"
#include "Resources\ResourceManager.h"
#include "Rendering\MeshRenderer.h"

#include "IO\ModelLoader.h"

// SFML AUDIO
#include "Audio\AudioListener.h"
#include "Audio\AudioEmitter.h"

// CUSTOM
#include "Custom Behaviours\MoveAround.h"
#include "Custom Behaviours\CameraOrbit.h"

// PHYSICS
#include "Physics\Rigidbody.h"

#include "imgui.h"
#include "examples\imgui_impl_glfw.h"
#include "examples\imgui_impl_opengl3.h"

Scene Graphyte::currentScene;

GLFWwindow* Graphyte::mainWindow;

GameObject* Graphyte::selectedGameObject;

// timing
float lastFrame = 0.0f;

static float frameTimes[500];
static int frameTimeOffset = 0;

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

	setupCallbacks();
}

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
int currentFrame = 0;


// TODO: Move somewhere more appropriate
double lastTime = glfwGetTime();
float lastFPSCount = 0;
float frameTime = 0;
int nbFrames = 0;
float inputTime = 0;

void Graphyte::mainLoop() 
{
	currentScene.OnSceneLoad();

	glCullFace(GL_FRONT);

	float previousdt = 0;
	float currentdt = 0;

	float dt = 0.01f; // 1/100 of a second
	float currentTime = glfwGetTime();
	float accumulator = 0.0f;

	while (!glfwWindowShouldClose(mainWindow)) {
		// Deltatime		
		float currentTime = 0; // This accumulates the time that has passed.
		float previousTime = 0;

		currentTime = glfwGetTime();
		Time::deltaTime = currentTime - previousTime; 	// UPDATE DELTATIME
		previousTime = currentTime;

		Input::UpdateKeyStates();
		Input::UpdateMousePosition();

		glfwPollEvents();

		// IMGUI
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		DrawUI();

		ImGui::Render();
		int display_w, display_h;
		glfwMakeContextCurrent(mainWindow);
		glfwGetFramebufferSize(mainWindow, &display_w, &display_h);
		glViewport(0, 0, display_w, display_h);
		glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		float newTime = glfwGetTime();
		float frameTime = newTime - currentTime;

		if (frameTime > 0.25f) 
		{
			frameTime = 0.25f;
		}
		currentTime = newTime;

		accumulator += frameTime;

		while (accumulator >= Time::fixedTimestep)
		{
			previousdt = currentdt;
			currentdt = Time::fixedTimestep;
			accumulator -= Time::fixedTimestep;

			currentScene.FixedUpdate();
		}

		float alpha = accumulator / dt;

		// Blending current and passed time
		Time::timeRemainder = currentdt * alpha + previousdt * (1.0f - alpha);

		Time::deltaTime = frameTime;

		// std::cout << "Physics updates per second: " << Time::deltaTime / Time::fixedDeltaTime << std::endl;

		// configure global opengl state
		// -----------------------------
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// TODO: Is it better if this is after or before Update()
		currentScene.CheckCollisions();

		// Scene Update()
		currentScene.Update();

		currentTime += Time::deltaTime;

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		frameTimes[currentFrame] = 1000.0f / ImGui::GetIO().Framerate;
		currentFrame++;

		if (currentFrame > 500) {
			currentFrame = 0;
		}

		glfwMakeContextCurrent(mainWindow);
		glfwSwapBuffers(mainWindow);
	}
}

bool show_demo_window = true;
bool show_transform_window = true;
bool show_camera_window = true;
bool show_scene_window = true;
bool backFaceCulling = false;
bool wireframe = false;
bool alpha = false;
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

		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Checkbox("Backface Culling", &backFaceCulling)) {
			if (backFaceCulling) 
			{
				glCullFace(GL_FRONT);
				glDisable(GL_CULL_FACE);
			}
			else 
			{
				glCullFace(GL_FRONT);
				glEnable(GL_CULL_FACE);
			}
		}

		/*
		if (ImGui::Button("Button")) // Buttons return true when clicked (most widgets return true when edited/activated)
		counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);
		*/

		if (ImGui::Checkbox("Wireframe", &wireframe)) {
			if (wireframe) {
				glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			}
			else {
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
		}

		if (ImGui::Checkbox("Alpha", &alpha)) {
			if (alpha) {
				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			}
			else {
				glDisable(GL_BLEND);
			}
		}

		ImGui::Checkbox("Grid Ready", &UniformGrid::gridReady);
		//ImGui::Checkbox("Grid Built", &UniformGrid::gridBuilt);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

		frameTimeOffset = (frameTimeOffset + 1) % IM_ARRAYSIZE(frameTimes);
		ImGui::PlotLines("Frame Times", frameTimes, IM_ARRAYSIZE(frameTimes), frameTimeOffset,"", 0.0f, 16.0F, ImVec2(0, 80));

		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_transform_window)
	{
		ImGui::Begin("Transform", &show_transform_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)

		if (selectedGameObject) {
			//ImGui::Text("Name: %s", selectedGameObject->transform.name.c_str());
			char *name = &selectedGameObject->transform.name[0u];

			//std::cout << IM_ARRAYSIZE(name) << std::endl;
			//std::cout << sizeof(selectedGameObject->transform.name) << std::endl;

			// TODO: Fixing a nasty bug that causes the program to crash whenever the length is more than 24 characters?
			ImGui::InputText("Name", name, 24);
			ImGui::Checkbox("Enabled", &selectedGameObject->enabled);

			ImGui::DragFloat3("Position", (float*)&selectedGameObject->transform.position);
			ImGui::DragFloat3("Rotation", (float*)&selectedGameObject->transform.rotation);
			ImGui::DragFloat3("Scale", (float*)&selectedGameObject->transform.scale);

			ImGui::Text("Parent: ", (float*)&selectedGameObject->transform.parent->name);

			ImGui::DragFloat3("Bounding Box Min", (float*)&selectedGameObject->transform.boundingBox.min);
			ImGui::DragFloat3("Bounding Box Max", (float*)&selectedGameObject->transform.boundingBox.max);

			static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
			int node_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
			ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3); // Increase spacing to differentiate leaves from expanded contents.
			
			selectedGameObject->DrawComponents();
			ImGui::PopStyleVar();

			// TODO: Making this dynamic instead
			if (ImGui::Button("Add AudioEmitter"))
			{
				selectedGameObject->AddComponent<AudioEmitter>("Assets/resources/Audio/testAudio.wav");
			}

			if (ImGui::Button("Add MoveAround"))
			{
				selectedGameObject->AddComponent<MoveAround>();
			}

			if (ImGui::Button("Add Rigidbody"))
			{
				selectedGameObject->AddComponent<Rigidbody>();
			}
		}

		ImGui::End();
	}

	if (show_scene_window) 
	{
		ImGui::Begin("Scene");
		if (ImGui::Button("Create Crysis Model"))
			currentScene.AddGameObject();

		if (ImGui::Button("Create Child GameObject"))
			currentScene.AddChild();

		if (ImGui::Button("Create 1000 Crysis Models"))
			currentScene.Add1000GameObjects();

		if (ImGui::Button("Create World"))
			currentScene.AddWorld();

		if (ImGui::Button("Create Grid"))
			currentScene.AddGrid();

		if (ImGui::Button("Create Grid Test Object"))
			currentScene.AddGridTestGameObject();

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
				currentScene.gameObjects[i]->DrawChildren();
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

		//currentScene.uniformGrid.DrawExtra();

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
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

// TODO: Moving these somewhere more appropriate
void Graphyte::loadShaders() 
{
	ResourceManager::LoadShader("Assets/shaders/Standard.vert", "Assets/shaders/Standard.frag", nullptr, "Standard");

	ResourceManager::LoadShader("Assets/shaders/Grid.vert", "Assets/shaders/Grid.frag", nullptr, "Grid");
	ResourceManager::GetShader("Grid").shaderName = "Grid";

	//ResourceManager::LoadTexture("Textures/Grid/texture_diffuse.psd", true, true, "Grid");
	ResourceManager::LoadTexture("Assets/textures/Grid/texture_diffuse.psd", false, true, "Grid");
	//ResourceManager::GetTexture("Grid").SetFiltering(false);
	// TODO: Move these somewhere more appropriate
	//glm::mat4 projection = glm::ortho(0.0f, static_cast<GLfloat>(1920), 0.0f, static_cast<GLfloat>(1080));
	//ResourceManager::LoadShader("Shaders/GUIText.vert", "Shaders/GUIText.frag", nullptr, "GUIText").SetMatrix4("projection", projection, GL_TRUE);
	//ResourceManager::LoadShader("Shaders/GUI.vert", "Shaders/GUI.frag", nullptr, "GUI").SetMatrix4("projection", projection, GL_TRUE);
	//projection = glm::perspective(glm::radians(65.0f), (float)1920 / (float)1080, 0.1f, 100.0f);
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
