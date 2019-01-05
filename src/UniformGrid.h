#pragma once

#include "BoundingBox.h"
#include "Objects\GameObject.h"
#include "ExtraRenderer.h"

#include <vector>
#include <queue>

#include <memory>

const int minSize = 8;

class UniformGrid {
private:
	// Working like intended
	UniformGrid(BoundingBox size, std::vector<GameObject*> gameObjectList);

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
public:
	BoundingBox boundaries;
	UniformGrid* parent;

	int curLife = -1;
	int maxLifeSpan = 8;

	// Child grids
	std::vector<UniformGrid*> childGrid = std::vector<UniformGrid*>(8);
	int activeChildren[8];
	int activeChildCount = 0;

	std::vector<GameObject*> gameObjects;					// GameObjects that this grid contains
	static std::queue<GameObject*> pendingGameObjects;		// GameObjects that will be added later

	static bool gridReady; // FALSE by default. the tree has a few objects which need to be inserted before it is complete 
	static bool gridBuilt; // FALSE by default. there is no pre-existing tree yet. 

	// Default constructor
	UniformGrid();

	// Constructor with bounding box size
	UniformGrid(int size);

	bool isRoot() {
		// Just check if it doesnt have a parent
		// Might be a bad idea, as you could manually assign it to be the root node somehow
		//return (parent == nullptr);

		return false;
	}

	bool InsertGameObject(GameObject* gameObject);

	void BuildGrid();		// Works
	void UpdateGrid();		// Not sure
	void Update();			// Not sure

	void DrawGrid() {
		ExtraRenderer::DrawAABB(boundaries, Vector3(0, 0, 0));

		for (int index = 0; index < 8; index++)
		{
			if (activeChildren[index] == 1)
			{
				childGrid[index]->DrawGrid();
			}
		}
	}

	void DrawExtra() 
	{
		for (int index = 0; index < 8; index++)
		{
			if (activeChildren[index] == 1)
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
	}
};