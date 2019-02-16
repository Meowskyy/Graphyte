#pragma once

#include "ECS.h"

#include "BoundingBox.h"

#include "Rendering\ExtraRenderer.h"

#include <vector>

const int minSize = 8;

class UniformGrid {
private:
	static float size;

	// Child grids
	std::vector<UniformGrid*> childGrid = std::vector<UniformGrid*>(8);
	bool activeChildren[8];
	int activeChildCount = 0;

	std::vector<GameObject*> gameObjects;	// GameObjects that this grid contains
	
	UniformGrid(const BoundingBox& boundaries) 
	{
		this->boundaries = boundaries;
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

	static std::vector<GameObject*> pendingGameObjects;		// GameObjects that will be added later
	static std::vector<std::string> collidingGameObjects;	// GameObjects that are touching currently

	static bool gridReady; // FALSE by default. the tree has a few objects which need to be inserted before it is complete 

	// Default constructor
	UniformGrid();

	// Constructor with bounding box boundaries
	UniformGrid(const int size);

	bool isRoot() const 
	{
		return (parent == nullptr);
	}

	bool InsertGameObject(GameObject& gameObject);

	void CollidingObjects(GameObject& gameObject) const;
	void CollidingObjects(GameObject& gameObject, const int index) const;

	void RebuildGrid();
	void Update();

	void DrawGrid() const;
	void DrawExtra() const;
};