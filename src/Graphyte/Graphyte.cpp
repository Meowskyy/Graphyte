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

// EDITOR COMPONENTS
#include "Custom Components\EditorCamera.h"

// SFML AUDIO
#include "Audio\AudioListener.h"
#include "Audio\AudioEmitter.h"

// CUSTOM
#include "Custom Components\MoveAround.h"
#include "Custom Components\CameraOrbit.h"

// PHYSICS
#include "Physics\Rigidbody.h"
#include "Physics\Collider.h"

#include "imgui.h"
#include "examples\imgui_impl_glfw.h"

#ifdef USE_VULKAN

#else
#include "examples\imgui_impl_opengl3.h" // OPENGL 
#endif

using namespace Graphyte;

Scene GraphyteEditor::currentScene;

GLFWwindow* GraphyteEditor::mainWindow;

GameObject* GraphyteEditor::selectedGameObject;

bool mouseOnGui = false;

// timing
float lastFrame = 0.0f;
static float frameTimes[500];
static int frameTimeOffset = 0;
int currentFrame = 0;

void GraphyteEditor::run() 
{
	initWindow();

	loadShaders();
	loadTextures();
	loadMaterials();
	loadModels();

	mainLoop();

	cleanup();
}

void GraphyteEditor::initWindow()
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
	//glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
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

	Screen::UpdateWindowSize(width, height);

	SetupIMGUI();

	setupCallbacks();
}

ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

// TODO: Move somewhere more appropriate
double lastTime = glfwGetTime();
float lastFPSCount = 0;
float frameTime = 0;
int nbFrames = 0;
float inputTime = 0;

// Physics loop based off https://gafferongames.com/post/fix_your_timestep/
void GraphyteEditor::mainLoop() 
{
	// Once the scene is loaded
	currentScene.OnSceneLoad();

	float previousState = 0;
	float currentState = 0;

	float currentTime = glfwGetTime();
	float accumulator = 0.0f;

	float distToSelectedObject = 10000;

	GameObject* editorCameraObject = new GameObject();
	editorCamera = &editorCameraObject->AddComponent<Camera>();
	editorCameraObject->AddComponent<EditorCamera>();
	editorCamera->transform->position = Vector3(0, 0, -20);
	Camera::mainCamera = editorCamera;

	while (!glfwWindowShouldClose(mainWindow)) 
	{
		mouseOnGui = ImGui::IsMouseHoveringAnyWindow();
		//std::cout << "START OF FRAME" << std::endl;
		Input::UpdateKeyStates();
		Input::UpdateMousePosition();

		glfwPollEvents();

		// IMGUI
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		DrawEditorUI();

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
			previousState = currentState;
			currentState = currentState + Time::fixedTimestep;
			currentState += Time::fixedTimestep;

			accumulator -= Time::fixedTimestep;

			currentScene.FixedUpdate();
		}

		float alpha = accumulator / Time::fixedTimestep;

		Time::deltaTime = frameTime;

		// TODO: Blending physics based on alpha
		// std::cout << "Remainder: " << alpha << "\n";
		// std::cout << "Physics updates per second: " << Time::deltaTime / Time::fixedDeltaTime << std::endl;

		// TODO: Is it better if this is after or before Update()
		currentScene.CheckCollisions();

		currentScene.Update();
		currentScene.LateUpdate();

		UpdateProjectionMatrix();

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

#ifndef EDITOR
		editorCameraObject->LateUpdate();
		currentScene.Render(*editorCamera);
#else
		currentScene.Render(*Camera::mainCamera);
#endif

		if (selectedGameObject)
			ExtraRenderer::DrawSelectionArrows(&selectedGameObject->transform);

		if (Input::GetMouseButtonPressed(0) && !mouseOnGui) 
		{
			selectedGameObject = nullptr;
			GameObject* closestObject = nullptr;
			float distToClosest = 100000;

			for (int i = 0; i < currentScene.gameObjects.size(); i++) {
				float intersection_distance; // Output of TestRayOBBIntersection()

				for (int j = 0; j < currentScene.gameObjects[i]->children.size(); j++)
				{
					Vector3 aabb_min = currentScene.gameObjects[i]->children[j]->transform.boundingBox.min;
					Vector3 aabb_max = currentScene.gameObjects[i]->children[j]->transform.boundingBox.max;

					// The ModelMatrix transforms :
					// - the mesh to its desired position and orientation
					// - but also the AABB (defined with aabb_min and aabb_max) into an OBB
					Matrix4 RotationMatrix = glm::mat4_cast(currentScene.gameObjects[i]->children[j]->transform.rotation);
					Matrix4 TranslationMatrix = glm::translate(Matrix4(), currentScene.gameObjects[i]->children[j]->transform.position);
					//Matrix4 ScaleMatrix = glm::scale(Matrix4(), currentScene.gameObjects[i]->children[j]->transform.scale);
					Matrix4 ModelMatrix = TranslationMatrix * RotationMatrix;

					if (Physics::TestRayOBBIntersection(editorCamera->transform->position, Physics::RaycastMouseDirection(*editorCamera), aabb_min, aabb_max, ModelMatrix, intersection_distance))
					{
						if (closestObject == nullptr || intersection_distance < distToClosest)
						{
							closestObject = currentScene.gameObjects[i]->children[j];
							selectedGameObject = currentScene.gameObjects[i]->children[j];

							distToClosest = glm::distance(selectedGameObject->transform.position, editorCamera->transform->position);
							distToSelectedObject = glm::distance(selectedGameObject->transform.position, editorCamera->transform->position);
						}
					}
				}

				Vector3 aabb_min = currentScene.gameObjects[i]->transform.boundingBox.min;
				Vector3 aabb_max = currentScene.gameObjects[i]->transform.boundingBox.max;

				// The ModelMatrix transforms :
				// - the mesh to its desired position and orientation
				// - but also the AABB (defined with aabb_min and aabb_max) into an OBB
				Matrix4 RotationMatrix = glm::mat4_cast(currentScene.gameObjects[i]->transform.rotation);
				Matrix4 TranslationMatrix = glm::translate(Matrix4(), currentScene.gameObjects[i]->transform.position);
				//Matrix4 ScaleMatrix = glm::scale(Matrix4(), currentScene.gameObjects[i]->transform.scale);
				Matrix4 ModelMatrix = TranslationMatrix * RotationMatrix;

				if (Physics::TestRayOBBIntersection(editorCamera->transform->position, Physics::RaycastMouseDirection(*editorCamera), aabb_min, aabb_max, ModelMatrix, intersection_distance))
				{
					if (i > 2) {
						if (closestObject == nullptr || intersection_distance < distToClosest)
						{
							closestObject = currentScene.gameObjects[i];
							selectedGameObject = currentScene.gameObjects[i];

							distToClosest = glm::distance(selectedGameObject->transform.position, editorCamera->transform->position);
							distToSelectedObject = glm::distance(selectedGameObject->transform.position, editorCamera->transform->position);
						}
					}
				}

				if (closestObject)
					selectedGameObject = closestObject;

				ExtraRenderer::DrawLine(currentScene.gameObjects[i]->transform.position, Physics::RaycastMouseDirection(*editorCamera));
			}
		}

		if (Input::GetMouseButtonDown(0) && !mouseOnGui) 
		{
			if (selectedGameObject) 
			{
				//selectedGameObject->transform.position = Physics::RaycastMousePosition(distToSelectedObject);
			}
		}

		//std::cout << "END OF FRAME" << std::endl;

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		frameTimes[currentFrame] = 1000.0f / ImGui::GetIO().Framerate;
		currentFrame++;

		if (currentFrame > 500) {
			currentFrame = 0;
		}

		glfwMakeContextCurrent(mainWindow);
		glfwSwapBuffers(mainWindow);
	}

	//delete editorCameraObject;
	//editorCameraObject = nullptr;
}

bool show_demo_window = true;
bool show_transform_window = true;
bool show_camera_window = true;
bool show_scene_window = true;
bool backFaceCulling = false;
bool wireframe = false;
bool alpha = false;
void GraphyteEditor::DrawEditorUI()
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
				glCullFace(GL_BACK);
				glEnable(GL_CULL_FACE);
			}
			else 
			{
				glCullFace(GL_BACK);
				glDisable(GL_CULL_FACE);
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

		ImGui::Checkbox("Draw Scene Window", &show_scene_window);
		ImGui::Checkbox("Draw Transform Window", &show_transform_window);

		ImGui::Checkbox("Grid Ready", &UniformGrid::gridReady);
		ImGui::Checkbox("Mouse over GUI", &mouseOnGui);
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

			ImGui::DragFloat3("AABB Min", (float*)&selectedGameObject->transform.boundingBox.min);
			ImGui::DragFloat3("AABB Max", (float*)&selectedGameObject->transform.boundingBox.max);
			ImGui::DragFloat3("AABB Size", (float*)&selectedGameObject->transform.boundingBox.size);

			ImGui::DragFloat3("Bounds Min", (float*)&selectedGameObject->transform.boundingBox.bounds.min);
			ImGui::DragFloat3("Bounds Max", (float*)&selectedGameObject->transform.boundingBox.bounds.max);

			static int selection_mask = (1 << 2); // Dumb representation of what may be user-side selection state. You may carry selection state inside or outside your objects in whatever format you see fit.
			int node_clicked = -1;                // Temporary storage of what node we have clicked to process selection at the end of the loop. May be a pointer to your own node type, etc.
			ImGui::PushStyleVar(ImGuiStyleVar_IndentSpacing, ImGui::GetFontSize() * 3); // Increase spacing to differentiate leaves from expanded contents.
			
			selectedGameObject->DrawComponents();
			ImGui::PopStyleVar();

			// TODO: Making all of the following dynamic instead?
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

			if (ImGui::Button("Add Collider"))
			{
				selectedGameObject->AddComponent<Collider>();
			}
		}

		ImGui::End();
	}

	if (show_scene_window) 
	{
		ImGui::Begin("Scene");
		if (ImGui::Button("Create Planet"))
			currentScene.AddGameObject();

		if (ImGui::Button("Create Crysis Model"))
			currentScene.AddCrysisGameObject();

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

		if (ImGui::Button("Create Plane"))
			currentScene.AddPlane();

		if (ImGui::Button("Create Cube"))
			currentScene.AddCube();

		if (ImGui::Button("Add Particle System"))
			currentScene.AddParticleSystemObject();

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

		currentScene.uniformGrid.DrawExtra();

		ImGui::End();
	}
}

void GraphyteEditor::SetupIMGUI() 
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

void GraphyteEditor::cleanup() 
{
	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
}

// TODO: Moving these somewhere more appropriate
void GraphyteEditor::loadShaders() 
{
	// EDITOR STUFF
	ResourceManager::LoadShader("Assets/shaders/Grid.vert", "Assets/shaders/Grid.frag", nullptr, "Grid");
	//ResourceManager::LoadShader("Assets/shaders/SimpleDepth.vert", "Assets/shaders/SimpleDepth.frag", nullptr, "SimpleDepth");
	ResourceManager::LoadShader("Assets/shaders/DepthDebug.vert", "Assets/shaders/DepthDebug.frag", nullptr, "DepthDebug");
	ResourceManager::LoadShader("Assets/shaders/ShadowMap.vert", "Assets/shaders/ShadowMap.frag", nullptr, "ShadowMap");

	// ACTUAL SHADERS
	//ResourceManager::LoadShader("Assets/shaders/PBR.vert", "Assets/shaders/PBR.frag", nullptr, "Standard");
	ResourceManager::LoadShader("Assets/shaders/Standard/StandardV2.vert", "Assets/shaders/Standard/StandardV2.frag", nullptr, "Standard");
	ResourceManager::LoadShader("Assets/shaders/Unlit.vert", "Assets/shaders/Unlit.frag", nullptr, "Unlit");
	//ResourceManager::LoadShader("Assets/shaders/Particles/Particles.vert", "Assets/shaders/Particles/Particles.frag", "Assets/shaders/Particles/Particles.geom", "Particles");
	ResourceManager::LoadShader("Assets/shaders/Particles/Particles.vert", "Assets/shaders/Particles/Particles.frag", nullptr, "Particles");



	//ResourceManager::LoadShader("Assets/shaders/Lighting.vert", "Assets/shaders/Lighting.frag", nullptr, "Lighting");

	//ResourceManager::LoadShader("Assets/shaders/ScreenShader.vert", "Assets/shaders/ScreenShader.frag", nullptr, "ScreenShader");
	//ResourceManager::GetShader("Grid").shaderName = "Grid";

	//ResourceManager::GetTexture("Grid").SetFiltering(false);
}

// TODO: Moving these somewhere more appropriate
void GraphyteEditor::loadTextures() 
{
	//ResourceManager::LoadTexture("Textures/Grid/texture_diffuse.psd", true, true, "Grid");
	ResourceManager::LoadTexture("Assets/textures/Grid/texture_diffuse.psd", false, true, "Grid");

	ResourceManager::LoadTexture("Assets/textures/GrassTexture.psd", false, false, "GrassTexture");
	ResourceManager::GetTexture("GrassTexture").SetFiltering(false);

	ResourceManager::LoadTexture("Assets/textures/particle.psd", false, true, "Particle");
}

// TODO: Moving these somewhere more appropriate
void GraphyteEditor::loadMaterials()
{
	ResourceManager::LoadMaterial("GridMaterial");
	ResourceManager::GetMaterial("GridMaterial").textures.push_back(ResourceManager::GetTexture("Grid"));
	ResourceManager::GetMaterial("GridMaterial").shader = ResourceManager::GetShader("Grid");
	
	ResourceManager::LoadMaterial("TerrainMaterial");
	ResourceManager::GetMaterial("TerrainMaterial").textures.push_back(ResourceManager::GetTexture("GrassTexture"));

	ResourceManager::LoadMaterial("LightingMat");
	ResourceManager::GetMaterial("LightingMat").shader = ResourceManager::GetShader("Lighting");

	ResourceManager::LoadMaterial("Particles");
	ResourceManager::GetMaterial("Particles").shader = ResourceManager::GetShader("Particles");
	ResourceManager::GetMaterial("Particles").textures.push_back(ResourceManager::GetTexture("Particle"));
}

// TODO: Moving these somewhere more appropriate
void GraphyteEditor::loadModels()
{
	//test = Model("Assets/resources/nanosuit/nanosuit.obj");

	//TextRenderer::Init();
	//TextRenderer::SetShader(ResourceManager::GetShader("GUIText"));
}

void GraphyteEditor::setupCallbacks()
{
	// SET CALLBACKS

	// INPUT
	glfwSetCursorPosCallback(mainWindow, Input::mouse_callback);
	glfwSetMouseButtonCallback(mainWindow, Input::mouse_button_callback);
	glfwSetKeyCallback(mainWindow, Input::key_callback);

	// WINDOW
	glfwSetWindowSizeCallback(mainWindow, window_size_callback);
	glfwSetFramebufferSizeCallback(mainWindow, framebuffer_size_callback);
}

void GraphyteEditor::glfw_error_callback(int error, const char* description)
{
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void GraphyteEditor::window_size_callback(GLFWwindow* window, int width, int height)
{
	std::cout << "New size: " << width << "x" << height << "\n";

	Screen::UpdateWindowSize(width, height);

	UpdateProjectionMatrix();
}

void GraphyteEditor::UpdateProjectionMatrix()
{
	ResourceManager::UpdateProjection(Camera::mainCamera->GetProjectionMatrix());
}

void GraphyteEditor::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	std::cout << "New framebuffer size: " << width << "x" << height << "\n";

	//Matrix4 projection = glm::perspective(Camera::mainCamera->fov, (float)width / (float)height, 0.1f, 1000.0f);
	//ResourceManager::UpdateProjection(projection);

	glViewport(0, 0, width, height);
}
