#pragma once

#include "Objects\GameObject.h"
#include "Resources\ResourceManager.h"
#include "Rendering\Camera.h"

#include "UniformGrid.h"

class Scene {
public:
	static std::vector<GameObject*> gameObjects;
	static UniformGrid uniformGrid;

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
	void AddGridTestGameObject();

	static GameObject* Instantiate(GameObject *original);
	static GameObject* Instantiate(GameObject *original, GameObject *parent);
	static GameObject* Instantiate(GameObject *original, Vector3 position);

	std::vector<GameObject*> GetAllRootObjects();
};