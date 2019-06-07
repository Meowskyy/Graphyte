#include "Scene.h"

#include "ECS.h"
#include "Custom Behaviours\CameraOrbit.h"
#include "Audio\AudioListener.h"
#include "Rendering\Camera.h"
#include "Rendering\MeshRenderer.h"
#include "Custom Behaviours\WorldGenerator.h"

#include "IO\ModelLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Rendering\Lighting.h"

#include "Rendering/Screen.h"

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
	m_shadowMap.Init(56, 56);
	m_lighting = new Lighting();
	m_lighting->Init();

	AddLight();
	AddCameraObject();

	ResourceManager::GetShader("Standard").SetInteger("mainTexture", 0);
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
}

// Update on Components
void Scene::Update()
{
	// Call Update on all components
	for (auto& gameObject : gameObjects)
	{
		gameObject->Update();
	}
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

void Scene::CheckCollisions()
{
	for (auto& gameObject : gameObjects)
	{
		gameObject->CheckCollisions();
	}
}

void Scene::Render(Camera& camera)
{
	ResourceManager::GetShader("Grid").SetVector3f("cameraPosition", camera.transform->position, true);
	ResourceManager::GetShader("Standard").SetVector3f("cameraPosition", camera.transform->position, true);

	RenderDepth(camera);

	uniformGrid.DrawGrid();

	RenderObjects(camera);

	// FOR DEBUGGING LATER
	//Renderer::RenderAllWithShader(ResourceManager::GetMaterial("Unlit"));
	//Renderer::RenderAllWithShader(ResourceManager::GetMaterial("Standard"));
	//Renderer::RenderAllWithShader(ResourceManager::GetMaterial("Unlit"));
}

void Scene::RenderDepth(Camera& camera) 
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	// 1. render depth of scene to texture (from light's perspective)
	// --------------------------------------------------------------
	glm::mat4 lightProjection, lightView;
	glm::mat4 lightSpaceMatrix;
	lightProjection = glm::ortho(-light->size, light->size, -light->size, light->size, light->nearClipPlane, light->farClipPlane);

	Vector3 position = Vector3(light->transform->position.x, light->transform->position.y, light->transform->position.z);
	Vector3 upVector = Vector3(light->transform->getUpVector().x, light->transform->getUpVector().y, light->transform->getUpVector().z);
	Vector3 forwardVector = Vector3(light->transform->getForwardVector().x, light->transform->getForwardVector().y, light->transform->getForwardVector().z);

	lightView = glm::lookAt(position, position + forwardVector, upVector);

	lightSpaceMatrix = lightProjection * lightView;
	// render scene from light's point of view
	ResourceManager::GetShader("ShadowMap").Use();
	ResourceManager::GetShader("ShadowMap").SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);

	m_shadowMap.BindForWriting();

	glClear(GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	Renderer::RenderAllDepth(camera);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// render scene from light's point of view
	ResourceManager::GetShader("Standard").Use();
	ResourceManager::GetShader("Standard").SetMatrix4("lightSpaceMatrix", lightSpaceMatrix);
}

void Scene::RenderObjects(Camera & camera)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_shadowMap.BindForReading(GL_TEXTURE1);

	m_lighting->SetDirectionalLight(*light);

	//glDisable(GL_CULL_FACE);
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
			vertices.push_back(Vector3(x - 0.5f, 0, z - 0.5f));
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
		2, 3, 1,
		2, 1, 0
	};

	//object->AddComponent<Collider>();

	meshRenderer->RecalculateBoundingBox();
	
	mesh->RecalculateNormals();
	mesh->SetupMesh();
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
		// front
		Vector3(-1.0, -1.0,  1.0),
		Vector3(1.0, -1.0,  1.0),
		Vector3(1.0,  1.0,  1.0),
		Vector3(-1.0,  1.0,  1.0),
		// back
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

	//object->AddComponent<Collider>();

	meshRenderer->RecalculateBoundingBox();
	
	mesh->RecalculateNormals();
	mesh->SetupMesh();
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

void Graphyte::Scene::AddLight()
{
	GameObject* object = Instantiate(new GameObject());
	object->transform.position = Vector3(-2.0f, 4.0f, -1.0f);

	light = &object->AddComponent<Light>();
	light->transform->name = "Directional Light";
}

void Scene::AddCameraObject()
{
	std::cout << "Added camera" << std::endl;

	GameObject* object = Instantiate(new GameObject());

	object->transform.name = "Main Camera";
	object->transform.position = Vector3(0, 10, 10);
	object->transform.rotation = Quaternion(Vector3(0, PI, 0));

	object->hasCollision = false;

	Camera::mainCamera = &object->AddComponent<Camera>();
	object->AddComponent<AudioListener>();

	object = nullptr;
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
	object->hasCollision = false;

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

	if (uniformGrid.gridReady) 
	{
		uniformGrid.InsertGameObject(*gameObjects[gameObjects.size() - 1]);
		uniformGrid.pendingGameObjects.push_back(gameObjects[gameObjects.size() - 1]);
	}

	return gameObjects[gameObjects.size() - 1];
}

GameObject* Scene::Instantiate(GameObject* original, GameObject& parent) {
	parent.AddChild(*original);
	original->transform.parent = &parent.transform;

	if (uniformGrid.gridReady)
	{
		uniformGrid.pendingGameObjects.push_back(parent.children[parent.children.size() - 1]);
	}

	return parent.children[parent.children.size() - 1];
}

GameObject* Scene::Instantiate(GameObject* original, const Vector3 &position) {
	original->transform.position = position;
	gameObjects.push_back(original);

	if (uniformGrid.gridReady)
	{
		uniformGrid.InsertGameObject(*gameObjects[gameObjects.size() - 1]);
		uniformGrid.pendingGameObjects.push_back(gameObjects[gameObjects.size() - 1]);
	}

	return gameObjects[gameObjects.size() - 1];
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