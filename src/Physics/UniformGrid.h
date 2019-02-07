#pragma once

#include "ECS.h"

#include "BoundingBox.h"

#include "Rendering\ExtraRenderer.h"

#include <vector>

const int minSize = 16;

class UniformGrid {
private:
	static float size;

	// Child grids
	std::vector<UniformGrid*> childGrid = std::vector<UniformGrid*>(8);
	bool activeChildren[8];
	int activeChildCount = 0;

	std::vector<GameObject*> gameObjects;	// GameObjects that this grid contains
	
	UniformGrid(const BoundingBox& size) 
	{
		this->boundaries = size;
	}

	// Create with only boundaries
	UniformGrid* CreateNode(const BoundingBox& boundary)
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

	static std::vector<GameObject*> pendingGameObjects;		// GameObjects that will be added later
	static std::vector<std::string> collidingGameObjects;	// GameObjects that are touching currently

	static bool gridReady; // FALSE by default. the tree has a few objects which need to be inserted before it is complete 

	// Default constructor
	UniformGrid();

	// Constructor with bounding box size
	UniformGrid(const int size);

	bool isRoot() {
		// Just check if it doesnt have a parent
		// Might be a bad idea, as you could manually assign it to be the root node somehow
		return (parent == nullptr);
	}

	bool InsertGameObject(GameObject& gameObject);
	void CollidingObjects(GameObject& gameObject);

	void RebuildGrid();					// Works
	void Update();						// Works
	void SetSize(int size);				// Do not call this more than once

	void DrawGrid();
	void DrawExtra();
};