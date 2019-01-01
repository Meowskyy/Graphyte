#pragma once

#include "GameObject.h"
#include "ResourceManager.h"
#include "Camera.h"

class Scene {
public:
	static std::vector<GameObject*> gameObjects;

	Scene(){
	}

	// UPDATING BEHAVIOURSCRIPTS
	void OnSceneLoad();	// Run on scene load?
	void Update();		// Run after every frame
	void FixedUpdate(); // Used for physics?

	// TODO: All these are temporary
	void AddGameObject();
	void AddChild();
	void Add1000GameObjects();
	void AddCameraObject();
	void AddWorld();
	void AddGrid();

	static GameObject* Instantiate(GameObject *original);
	static GameObject* Instantiate(GameObject *original, GameObject *parent);

	std::vector<GameObject*> GetAllRootObjects();
};