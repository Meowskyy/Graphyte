#include "Scene.h"

#include "CameraOrbit.h"
#include "AudioListener.h"

#include "ModelLoader.h"
#include "MeshRenderer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Camera.h"

// Camera
Camera *Scene::mainCamera;
std::vector<GameObject*> Scene::gameObjects;

void Scene::Update()
{
	// Set OpenGL options
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// TODO: IMPORTANT Moving this somewhere else and only updating when necessary
	// Or is this actually doable?
	Shader objShader = ResourceManager::GetShader("Standard").Use();
	objShader.SetMatrix4("view", Camera::mainCamera->GetViewMatrix());

	// Looping through all the GameObjects
	for (int i = 0; i < gameObjects.size(); i++)
	{
		// Looping through all the behaviours
		for (int behaviourCount = 0; behaviourCount < gameObjects[i]->behaviour.size(); behaviourCount++)
		{
			gameObjects[i]->behaviour.at(behaviourCount)->Update();
		}
	}
}

// Called multiple times per frame
// Should be used for physics
void Scene::FixedUpdate()
{
	for (int i = 0; i < gameObjects.size(); i++) {
		for (int behaviourCount = 0; behaviourCount < gameObjects[i]->behaviour.size(); behaviourCount++) {
			gameObjects[i]->behaviour.at(behaviourCount)->FixedUpdate();
		}
	}
}



// TODO: Move this somewhere more appropriate
void Scene::AddGameObject()
{
#define TEST
#ifndef TEST
	gameObjects.push_back(new GameObject());

	std::string newName = "GameObject " + std::to_string(gameObjectCount);
	strcpy(gameObjects[gameObjectCount]->transform.name, newName.c_str());

	gameObjects[gameObjectCount]->AddBehaviour(new MeshRenderer(gameObjects[gameObjectCount]->transform));


	gameObjects[gameObjectCount]->transform.position.x = gameObjectCount * 2;

	gameObjectCount++;
#else
	ModelLoader::loadGameObject("Assets/resources/nanosuit/nanosuit.obj");

	//std::string newName = "GameObject " + std::to_string(gameObjects.size() - 1);
	//strcpy(gameObjects[gameObjects.size() - 1]->transform.name, newName.c_str());

	gameObjects[gameObjects.size() - 1]->transform.position.x = (gameObjects.size() - 1) * 2;
#endif // TEST
}

void Scene::AddChild() {
	//Graphyte::selectedGameObject->transform.children.push_back(new GameObject());

	//std::string newName = "GameObject " + std::to_string(gameObjectCount);
	//strcpy(gameObjects[gameObjectCount]->transform.name, newName.c_str());

	//gameObjects[gameObjectCount]->AddBehaviour(new MeshRenderer(gameObjects[gameObjectCount]->transform));


	//gameObjects[gameObjectCount]->transform.position.x = gameObjectCount * 2;

	//gameObjectCount++;
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

	gameObjects.push_back(new GameObject());

	gameObjects[gameObjects.size() - 1]->transform.name = "Main Camera";
	gameObjects[gameObjects.size() - 1]->AddBehaviour(new CameraOrbit(gameObjects[gameObjects.size() - 1]->transform));
	gameObjects[gameObjects.size() - 1]->AddBehaviour(new Camera(gameObjects[gameObjects.size() - 1]->transform));
	gameObjects[gameObjects.size() - 1]->AddBehaviour(new AudioListener(gameObjects[gameObjects.size() - 1]->transform));

	Camera::mainCamera = (Camera*)(gameObjects[gameObjects.size() - 1]->behaviour.at(0));
}
