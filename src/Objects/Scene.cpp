#include "Scene.h"

#include "ECS.h"
#include "Custom Components\CameraOrbit.h"
#include "Audio\AudioListener.h"
#include "Rendering\Camera.h"
#include "Rendering\MeshRenderer.h"
#include "Custom Components\WorldGenerator.h"

#include "IO\ModelLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Rendering/Lighting.h"

#include "Rendering/Screen.h"

#include "Physics/Rigidbody.h"
#include "Physics/Collider.h"

#include "Rendering\ParticleSystem.h"

using namespace Graphyte;

// Camera
std::vector<GameObject*> Scene::gameObjects;
UniformGrid Scene::uniformGrid;

Scene::~Scene() 
{
	std::cout << "Cleaning up Scene\n";
	// Call Update on all components
	for (auto& gameObject : gameObjects)
	{
		delete gameObject;
	}

	delete m_lighting;
	m_lighting = nullptr;

	delete light->gameObject;
	light = nullptr;
}

void Scene::OnSceneLoad()
{
	m_shadowMap.Init(512, 512);
	m_lighting = new Lighting();
	m_lighting->Init();

	AddLight();
	AddCameraObject();

	ResourceManager::GetShader("Standard").Use().SetInteger("mainTexture", 0);
	ResourceManager::GetShader("Standard").SetInteger("shadowMap", 1);

#ifdef _DEBUG
	//AddGrid();
#endif

	// Call OnSceneLoad on all components
	for (auto& gameObject : gameObjects)
	{
		gameObject->OnSceneLoad();
	}

	uniformGrid = UniformGrid(32);
	uniformGrid.RebuildGrid();

	AddPlane();
	//AddParticleSystemObject();
}

// Update on Components
void Scene::Update()
{
	auto updateStart = std::chrono::high_resolution_clock::now();

	// Call Update on all components
	for (auto& gameObject : gameObjects)
	{
		gameObject->Update();
	}

	auto updateEnd = std::chrono::high_resolution_clock::now();

	//std::cout << "Time elapsed in Update: " << std::chrono::duration_cast<std::chrono::nanoseconds>(updateEnd - updateStart).count() << " ns\n";
}

// LateUpdate on Components
void Scene::LateUpdate()
{
	// Call LateUpdate on all components
	for (auto& gameObject : gameObjects)
	{
		gameObject->LateUpdate();
	}
}

// FixedUpdate on Components
void Scene::FixedUpdate()
{
	for (auto& gameObject : gameObjects) 
	{
		gameObject->FixedUpdate();
	}

	uniformGrid.Update();
}

void Scene::Render(Camera& camera)
{
	ResourceManager::GetShader("Grid").SetVector3f("cameraPosition", camera.transform->position, true);
	ResourceManager::GetShader("Standard").SetVector3f("cameraPosition", camera.transform->position, true);

	RenderDepth(camera);

	glViewport(0, 0, Screen::width, Screen::height);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#ifdef EDITOR
	uniformGrid.DrawGrid();
#endif

	RenderObjects(camera);

	// FOR DEBUGGING LATER
	//Renderer::RenderAllWithShader(ResourceManager::GetMaterial("Unlit"));
	//Renderer::RenderAllWithShader(ResourceManager::GetMaterial("Standard"));
	//Renderer::RenderAllWithShader(ResourceManager::GetMaterial("Unlit"));

	//std::vector<Renderer*> objectsToRender = uniformGrid.GetObjectsToRender(camera);

	// TODO: Remove after checking stuff
	//uniformGrid.Update();

	ResourceManager::GetShader("ScreenShader").SetFloat("near_plane", light->nearClipPlane, true);
	ResourceManager::GetShader("ScreenShader").SetFloat("far_plane", light->farClipPlane);

	//m_shadowMap.BindForReading(GL_TEXTURE1);
	//ExtraRenderer::DrawTextureOnScreen(m_shadowMap.framebufferTexture);
}

void Scene::RenderDepth(Camera& camera) 
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);

	Matrix4 lightSpaceMatrix = light->GetLightSpaceMatrix();
	ResourceManager::GetShader("ShadowMap").SetMatrix4("lightSpaceMatrix", lightSpaceMatrix, true);
	ResourceManager::currentShader = ResourceManager::GetShader("ShadowMap");

	m_shadowMap.BindForWriting();

	Renderer::RenderAllDepth(camera);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	ResourceManager::GetShader("Standard").SetMatrix4("lightSpaceMatrix", lightSpaceMatrix, true);
}

void Scene::RenderObjects(Camera& camera)
{
	m_shadowMap.BindForReading(GL_TEXTURE1);

	m_lighting->SetDirectionalLight(*light);

	glCullFace(GL_BACK);
	Renderer::RenderAllGrouped(camera);
}

// TODO: Move this somewhere more appropriate
void Scene::AddGameObject()
{
	//ModelLoader::loadGameObject("Assets/resources/nanosuit/nanosuit.obj");
	ModelLoader::loadGameObject("Assets/resources/planet/planet.obj");
}

// TODO: Move this somewhere more appropriate
void Scene::AddCrysisGameObject()
{
	ModelLoader::loadGameObject("Assets/resources/nanosuit/nanosuit.obj");
}

// TODO: Move this somewhere more appropriate
void Scene::AddGridTestGameObject()
{
	Instantiate(new GameObject(), Vector3(-6, 6, -6));
}

void Scene::AddPlane()
{
	GameObject* object = Instantiate(new GameObject());
	object->transform.name = "Plane";

	object->transform.scale = Vector3(10, 1, 10);
	object->transform.position = Vector3(0, -2, 0);

	// MESH RENDERER
	MeshRenderer* meshRenderer = &object->AddComponent<MeshRenderer>();
	meshRenderer->SetMaterial(ResourceManager::GetMaterial("Standard"));

	// SETUP THE MESH
	Mesh* mesh = &meshRenderer->mesh;
	std::vector<Vector3> vertices;
	// MESH
	// VERTICES
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			vertices.push_back(Vector3(-x + 0.5f, 0, z - 0.5f));
		}
	}

	mesh->vertices = vertices;

	mesh->uvs = std::vector<Vector2>
	{
		Vector2(0, 0),
		Vector2(1, 0),
		Vector2(0, 1),
		Vector2(1, 1),
	};

	mesh->indices = std::vector<unsigned int>
	{
		2, 1, 3,
		2, 0, 1
	};

	meshRenderer->RecalculateBoundingBox();
	
	mesh->RecalculateNormals();
	mesh->SetupMesh();

	object->AddComponent<Collider>();
}

void Scene::AddCube()
{
	GameObject* object = Instantiate(new GameObject());
	object->transform.name = "Cube";

	// MESH RENDERER
	MeshRenderer* meshRenderer = &object->AddComponent<MeshRenderer>();
	meshRenderer->SetMaterial(ResourceManager::GetMaterial("Standard"));

	// SETUP THE MESH
	Mesh* mesh = &meshRenderer->mesh;

	mesh->vertices = std::vector<Vector3>
	{
		Vector3(-1.0, -1.0,  1.0),
		Vector3(1.0, -1.0,  1.0),
		Vector3(1.0,  1.0,  1.0),
		Vector3(-1.0,  1.0,  1.0),

		Vector3(-1.0, -1.0, -1.0),
		Vector3(1.0, -1.0, -1.0),
		Vector3(1.0,  1.0, -1.0),
		Vector3(-1.0,  1.0, -1.0)
	};

	mesh->indices = std::vector<unsigned int>
	{
		// front
		0, 1, 2,
		2, 3, 0,
		// right
		1, 5, 6,
		6, 2, 1,
		// back
		7, 6, 5,
		5, 4, 7,
		// left
		4, 0, 3,
		3, 7, 4,
		// bottom
		4, 5, 1,
		1, 0, 4,
		// top
		3, 2, 6,
		6, 7, 3
	};

	meshRenderer->RecalculateBoundingBox();
	
	mesh->RecalculateNormals();
	mesh->SetupMesh();

	object->AddComponent<Collider>();

	object->AddComponent<Rigidbody>();
}

void Scene::AddSphere()
{
}

void Scene::AddChild() {
	Instantiate(new GameObject(), *GraphyteEditor::selectedGameObject);
}

void Scene::Add1000GameObjects()
{
	for (int i = 0; i < 1000; i++) {
		AddGameObject();
	}
}

void Graphyte::Scene::AddParticleSystemObject()
{
	GameObject* object = Instantiate(new GameObject());
	object->transform.name = "Particle System";

	// MESH RENDERER
	ParticleSystem* particleSystem = &object->AddComponent<ParticleSystem>();
	particleSystem->SetMaterial(ResourceManager::GetMaterial("Particles"));

	particleSystem->Init();
}

void Graphyte::Scene::AddLight()
{
	GameObject* object = Instantiate(new GameObject());
	object->transform.position = Vector3(-2.0f, 4.0f, -1.0f);

	light = &object->AddComponent<Light>();
	light->transform->name = "Directional Light";

	//object->AddComponent<MoveAround>();
}

void Scene::AddCameraObject()
{
	std::cout << "Added camera" << std::endl;

	GameObject* object = Instantiate(new GameObject());

	object->transform.name = "Main Camera";
	object->transform.position = Vector3(0, 10, 10);
	object->transform.rotation = Quaternion(Vector3(0, PI, 0));

	Camera::mainCamera = &object->AddComponent<Camera>();
	//object->AddComponent<Camera>();
	object->AddComponent<AudioListener>();

#ifndef EDITOR
	object->AddComponent<CameraOrbit>();
#endif
}

std::vector<GameObject*> Scene::GetAllRootObjects() const
{
	std::vector<GameObject*> objects;
	return objects;
}

void Scene::AddGrid() 
{
	GameObject* object = Instantiate(new GameObject());

	object->transform.name = "Grid";

	// MESH RENDERER
	MeshRenderer* meshRenderer = &object->AddComponent<MeshRenderer>();
	Mesh* mesh = &meshRenderer->mesh;
	meshRenderer->SetMaterial(ResourceManager::GetMaterial("GridMaterial"));

	std::vector<Vector3> vertices;
	// MESH
	// VERTICES
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			vertices.push_back(Vector3(x - 0.5f, 0, z - 0.5f));
		}
	}

	// UV SIZE
	float tilingAmount = 1000;

	object->transform.scale = Vector3(1, 1, 1) * tilingAmount;
	object->transform.position = Vector3(0, 0, 0);

	mesh->vertices = vertices;

	mesh->uvs = std::vector<Vector2>
	{
		Vector2(0, 0) * tilingAmount,
		Vector2(1, 0) * tilingAmount,
		Vector2(0, 1) * tilingAmount,
		Vector2(1, 1) * tilingAmount,
	};

	mesh->indices = std::vector<unsigned int>
	{
		2, 3, 1,
		2, 1, 0
	};

	mesh->SetupMesh();
}

void Scene::AddWorld() 
{
	std::cout << "Added WorldGenerator" << std::endl;

	GameObject* object = Instantiate(new GameObject());

	object->transform.name = "World Test";
	object->transform.scale = Vector3(1, 1, 1);
	object->AddComponent<WorldGenerator>();
}

GameObject* Scene::Instantiate(GameObject* original) {
	gameObjects.push_back(original);

	return original;
}

GameObject* Scene::Instantiate(GameObject* original, GameObject& parent) {
	parent.AddChild(original);
	original->transform.parent = &parent.transform;

	return original;
}

GameObject* Scene::Instantiate(GameObject* original, const Vector3 &position) {
	original->transform.position = position;
	gameObjects.push_back(original);

	return original;
}

void Scene::Destroy(GameObject* original)
{
	delete original;
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), nullptr), gameObjects.end());
}

void Scene::DestroyLast()
{
	delete gameObjects[gameObjects.size() - 1];
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), nullptr), gameObjects.end());
}