#pragma once

#include "BoundingBox.h"
#include "Objects\GameObject.h"
#include "ExtraRenderer.h"

#include <vector>

const int minSize = 16;

class UniformGrid {
private:
	static float size;
	
	// Working like intended
	UniformGrid(BoundingBox size, std::vector<GameObject*> gameObjectList);
	
	UniformGrid(BoundingBox size) 
	{
		this->boundaries = size;
	}

	UniformGrid* CreateNode(BoundingBox* boundary, std::vector<GameObject*> objList) //complete & tested
	{
		if (objList.size() == 0)
			return nullptr;

		UniformGrid* ret = new UniformGrid(*boundary, objList);
		ret->parent = this;
		return ret;
	}

	UniformGrid* CreateNode(BoundingBox boundary, GameObject* gameObject)
	{
		std::vector<GameObject*> objList(1); // sacrifice potential CPU time for a smaller memory footprint
		objList.push_back(gameObject);
		UniformGrid* ret = new UniformGrid(boundary, objList);
		ret->parent = this;
		return ret;
	}

	// Create with only boundaries
	UniformGrid* CreateNode(BoundingBox boundary)
	{
		UniformGrid* ret = new UniformGrid(boundary);
		ret->parent = this;
		return ret;
	}
public:
	BoundingBox boundaries;
	UniformGrid* parent;

	int curLife = -1;
	int maxLifeSpan = 8;

	// Child grids
	std::vector<UniformGrid*> childGrid = std::vector<UniformGrid*>(8);
	bool activeChildren[8];
	int activeChildCount = 0;

	std::vector<GameObject*> gameObjects;					// GameObjects that this grid contains
	static std::vector<GameObject*> pendingGameObjects;		// GameObjects that will be added later
	//static std::vector<GameObject*> collidingGameObjects;	// GameObjects that are touching currently
	static std::vector<std::string> collidingGameObjects;	// GameObjects that are touching currently

	static bool gridReady; // FALSE by default. the tree has a few objects which need to be inserted before it is complete 

	// Default constructor
	UniformGrid();

	// Constructor with bounding box size
	UniformGrid(int size);

	bool isRoot() {
		// Just check if it doesnt have a parent
		// Might be a bad idea, as you could manually assign it to be the root node somehow
		return (parent == nullptr);

		//return false;
	}

	bool InsertGameObject(GameObject* gameObject); // Bugged

	void CollidingObjects(GameObject* gameObject);

	void RebuildGrid();					// Works
	void Update();						// Works
	void SetSize(int size);				// Do not call this more than once

	void DrawGrid() {
		ExtraRenderer::DrawUniformBox(boundaries);

		for (int index = 0; index < 8; index++)
		{
			if (activeChildren[index] == true)
			{
				childGrid[index]->DrawGrid();
			}
		}
	}

	void DrawExtra() 
	{
		for (int index = 0; index < 8; index++)
		{
			if (activeChildren[index] == true)
			{
				childGrid[index]->DrawExtra();
			}
		}

		if (gameObjects.size() > 0) {
			ImGui::Text("Grid");
			ImGui::SameLine();
			int size = boundaries.max.x - boundaries.min.x;
			ImGui::Text(std::to_string(size).c_str());
			ImGui::SameLine();
			ImGui::Text("Contains");
			for (int i = 0; i < gameObjects.size(); i++) {
				ImGui::Text("GameObject: ");
				ImGui::SameLine();

				if (gameObjects[i] != nullptr)
					ImGui::Text(gameObjects[i]->transform.name.c_str());
			}
		}

		if (isRoot()) {
			if (collidingGameObjects.size() > 0) {
				ImGui::Text("Colliding: ");
				for (int i = 0; i < collidingGameObjects.size(); i++) {
					//if (collidingGameObjects[i] != nullptr)
					ImGui::Text(collidingGameObjects[i].c_str());
					//ImGui::Text(collidingGameObjects[i]->transform.name.c_str());
				}
			}
		}
	}
};