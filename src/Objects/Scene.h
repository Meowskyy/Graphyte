#pragma once

#include "Objects\GameObject.h"
#include "Resources\ResourceManager.h"
#include "Rendering\Camera.h"

#include "UniformGrid.h"

class Scene {
public:
	static std::vector<GameObject*> gameObjects;
	static Camera* mainCamera;

	UniformGrid grid = UniformGrid();

	Scene(){
		grid = UniformGrid();
		grid.SetupGrip(1);
	}

	// UPDATING BEHAVIOURSCRIPTS
	void Update();		// Run after every frame
	void FixedUpdate(); // Used for physics?

	void AddGameObject();
	void AddChild();
	void Add1000GameObjects();
	void AddCameraObject();
};