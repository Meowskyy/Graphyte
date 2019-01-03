#include "Scene.h"

#include "Custom Behaviours\CameraOrbit.h"
#include "Audio\AudioListener.h"

#include "IO\ModelLoader.h"
#include "Rendering\MeshRenderer.h"
#include "Custom Behaviours\WorldGenerator.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Rendering\Camera.h"

#include "UniformGrid.h"

// Camera
std::vector<GameObject*> Scene::gameObjects;

UniformGrid* uniformGrid;

// TODO: Settings up OnSceneLoad function
void Scene::OnSceneLoad()
{
	AddGrid();

	uniformGrid = new UniformGrid();
	uniformGrid->SetupGrid(64);

	// UPDATING BEHAVIOURSCRIPTS AND RENDERING MESHES
	for (int i = 0; i < gameObjects.size(); i++)
	{
		gameObjects[i]->OnSceneLoad();
	}
}

void Scene::Update()
{
	ResourceManager::GetShader("Grid").SetVector3f("cameraPosition", Camera::mainCamera->transform->position, true);
	ResourceManager::GetShader("Grid").SetMatrix4("view", Camera::mainCamera->GetViewMatrix(), true);

	// TODO: IMPORTANT Moving this somewhere else and only updating when necessary
	ResourceManager::GetShader("Standard").SetMatrix4("view", Camera::mainCamera->GetViewMatrix(), true);

	// UPDATING BEHAVIOURSCRIPTS AND RENDERING MESHES
	for (int i = 0; i < gameObjects.size(); i++)
	{
		if (gameObjects[i]->hasCollision) {
			uniformGrid->CanFitIntoBoundaries(gameObjects[i]);
		}

		gameObjects[i]->Update();
	}

	uniformGrid->DrawGrid();
}

// FixedUpdate on BehaviourScripts
void Scene::FixedUpdate()
{
	for (int i = 0; i < gameObjects.size(); i++) {
		gameObjects[i]->FixedUpdate();
	}
}

// TODO: Move this somewhere more appropriate
void Scene::AddGameObject()
{
	ModelLoader::loadGameObject("Assets/resources/nanosuit/nanosuit.obj");
}

void Scene::AddChild() {
	Instantiate(new GameObject(), Graphyte::selectedGameObject);
}

void Scene::Add1000GameObjects()
{
	for (int i = 0; i < 1000; i++) {
		AddGameObject();
	}
}

void Scene::AddCameraObject()
{
	std::cout << "Added camera" << std::endl;

	GameObject* object = Instantiate(new GameObject());

	object->transform.name = "Main Camera";
	object->transform.position = Vector3(0, 10, 10);
	object->transform.rotation = Quaternion(Vector3(0, PI, 0));

	object->hasCollision = false;

	object->AddBehaviour(new CameraOrbit());
	object->AddBehaviour(new Camera());
	object->AddBehaviour(new AudioListener());

	Camera::mainCamera = (Camera*)(object->GetBehaviour("Camera"));
	Camera::mainCamera->renderDistance = 1000;
}

std::vector<GameObject*> Scene::GetAllRootObjects()
{
	return gameObjects;
}

// TODO: Reworking the grid to fade out at distance
void Scene::AddGrid() 
{
	GameObject* object = Instantiate(new GameObject());

	//float gridScale = 1.0f;
	//object->transform.scale = glm::vec3(1, 1, 1) * gridScale;
	//object->transform.position = glm::vec3(-0.5f, 0, -0.5f);
	object->transform.name = "Grid";
	object->hasCollision = false;

	// MESH RENDERER
	object->AddBehaviour(new MeshRenderer());

	MeshRenderer* meshRenderer = (MeshRenderer*)object->GetBehaviour("MeshRenderer");
	Mesh* mesh = &meshRenderer->mesh;
	meshRenderer->materials[0].shader = ResourceManager::GetShader("Grid");
	meshRenderer->materials[0].textures.push_back(ResourceManager::GetTexture("Grid"));

	std::vector<Vector3> vertices;
	std::vector<unsigned int> indices;
	std::vector<Vector2> uvs;
	// MESH
	// VERTICES
	for (int z = 0; z < 2; z++) {
		for (int x = 0; x < 2; x++) {
			vertices.push_back(Vector3(x - 0.5f, 0, z - 0.5f));
		}
	}

	// INDICES
	indices.push_back(2);	//1
	indices.push_back(3);	//2
	indices.push_back(1);	//3
	indices.push_back(2);	//1
	indices.push_back(1);	//3
	indices.push_back(0);	//4

	// UVS
	// UV SIZE
	float tilingAmount = 1000;
	uvs.push_back(Vector2(0, 0) * tilingAmount);
	uvs.push_back(Vector2(1, 0) * tilingAmount);
	uvs.push_back(Vector2(0, 1) * tilingAmount);
	uvs.push_back(Vector2(1, 1) * tilingAmount);

	object->transform.scale = Vector3(1, 1, 1) * tilingAmount;
	object->transform.position = Vector3(0, 0, 0);

	mesh->vertices = vertices;
	mesh->uvs = uvs;
	mesh->indices = indices;

	mesh->setupMesh();
}

void Scene::AddWorld() 
{
	std::cout << "Added WorldGenerator" << std::endl;

	GameObject* object = Instantiate(new GameObject());

	object->transform.name = "World Test";
	object->AddBehaviour(new WorldGenerator());
}

GameObject* Scene::Instantiate(GameObject *original) {
	gameObjects.push_back(original);

	return gameObjects[gameObjects.size() - 1];
}

GameObject* Scene::Instantiate(GameObject *original, GameObject *parent) {
	parent->AddChild(*original);

	return &parent->children[parent->children.size() - 1];
}