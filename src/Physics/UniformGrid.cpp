#include "UniformGrid.h"

#include "imgui.h"

#include <iostream>

std::vector<GameObject*> UniformGrid::pendingGameObjects;		// GameObjects that will be added later
//std::vector<GameObject*> UniformGrid::collidingGameObjects;
std::vector<std::string> UniformGrid::collidingGameObjects;

bool UniformGrid::gridReady; // FALSE by default. the tree has a few objects which need to be inserted before it is complete 
float UniformGrid::size;

// Public
UniformGrid::UniformGrid() 
{
	boundaries = BoundingBox(Vector3(0, 0, 0), Vector3(0, 0, 0));
	curLife = -1;
}

// Public
UniformGrid::UniformGrid(const int size)
{
	int halfSize = size / 2;
	this->boundaries = BoundingBox(Vector3(-halfSize, -halfSize, -halfSize), Vector3(halfSize, halfSize, halfSize));
	this->parent = nullptr;

	curLife = -1;
}

bool UniformGrid::InsertGameObject(GameObject& gameObject)
{
	// If the object doesnt fit try inserting it in the parent grid
	if (!boundaries.Contains(gameObject.transform))
	{
		if (this->parent != nullptr)
		{
			// std::cout << "Trying to insert into parent" << std::endl;
			return this->parent->InsertGameObject(gameObject);
		}
		else
		{
			RebuildGrid();
			return false;
		}
	}

	Vector3 dimensions = boundaries.max - boundaries.min;
	if (dimensions.x <= minSize && dimensions.y <= minSize && dimensions.z <= minSize)
	{
		gameObjects.push_back(&gameObject);
		return true;
	}

	Vector3 half = (boundaries.max - boundaries.min) / 2.0f;
	Vector3 center = boundaries.min + half;

	BoundingBox childGrids[8];

	// Bottom 4
	childGrids[0] = BoundingBox(boundaries.min, center);																				// WORKS / TESTED
	childGrids[1] = BoundingBox(Vector3(center.x, boundaries.min.y, boundaries.min.z), Vector3(boundaries.max.x, center.y, center.z));	// WORKS / TESTED
	childGrids[2] = BoundingBox(Vector3(boundaries.min.x, boundaries.min.y, center.z), Vector3(center.x, center.y, boundaries.max.z));	// WORKS / TESTED
	childGrids[3] = BoundingBox(Vector3(center.x, boundaries.min.y, center.z), Vector3(boundaries.max.x, center.y, boundaries.max.z));	// WORKS / TESTED

	// Top 4
	childGrids[4] = BoundingBox(Vector3(boundaries.min.x, center.y, boundaries.min.z), Vector3(center.x, boundaries.max.y, center.z));	// WORKS / TESTED
	childGrids[5] = BoundingBox(Vector3(center.x, center.y, boundaries.min.z), Vector3(boundaries.max.x, boundaries.max.y, center.z));	// WORKS / TESTED
	childGrids[6] = BoundingBox(Vector3(boundaries.min.x, center.y, center.z), Vector3(center.x, boundaries.max.y, boundaries.max.z));	// WORKS / TESTED
	childGrids[7] = BoundingBox(center, boundaries.max);																				// WORKS / TESTED

	for (int childIndex = 0; childIndex < 8; childIndex++)
	{
		if (childGrids[childIndex].Contains(gameObject.transform))
		{
			//std::cout << "Inserted Into child: " << childIndex << " & Size: " << childGrids[childIndex].min.x << ", " << childGrids[childIndex].max.x << std::endl;
			childGrid[childIndex] = CreateNode(childGrids[childIndex]);
			childGrid[childIndex]->parent = this;

			activeChildren[childIndex] = true;
			activeChildCount++;

			// It only puts it into 1 grid and then returns
			return childGrid[childIndex]->InsertGameObject(gameObject);
		}
	}

	// std::cout << "Does not fit in any child \n";

	// std::cout << "Inserted Into root: " << (boundaries.max.x - boundaries.min.x) << std::endl;
	gameObjects.push_back(&gameObject);

	return false;
}

void UniformGrid::RebuildGrid()
{
	for (int i = 0; i < 8; i++)
	{
		activeChildren[i] = false;
		childGrid[i] = nullptr;
	}

	// Double grid size every time a object leaves bounds
	std::cout << "Maximum value for float: " << std::numeric_limits<float>::max() << '\n';
	std::cout << "Current Value: " << size << "\n";
	size *= 2;
	std::cout << "Resizing grid to: " << size << "\n";

	gameObjects.clear();

	int halfSize = size / 2;
	activeChildCount = 0;

	this->boundaries = BoundingBox(Vector3(-halfSize, -halfSize, -halfSize), Vector3(halfSize, halfSize, halfSize));

	// std::cout << "Pending gameObject count: " << pendingGameObjects.size() << "\n";

	for (int i = 0; i < pendingGameObjects.size(); i++) 
	{
		InsertGameObject(*pendingGameObjects[i]);
		// std::cout << "pending gameObject: " << pendingGameObjects[i]->transform.name << "\n";
	}

	gridReady = true;
}

void UniformGrid::Update() {
	if (gridReady)
	{
		// recursively update any child grids
		for (int childIndex = 0; childIndex < 8; childIndex++)
		{
			if (activeChildren[childIndex] == true)
			{
				childGrid[childIndex]->Update();
			}
		}

		for (int i = 0; i < gameObjects.size(); i++)
		{
			if (gameObjects[i]->transform.positionHasChanged())
			{	
				bool inserted = false;

				if (boundaries.Contains(gameObjects[i]->transform)) 
				{
					//std::cout << "Still fits inside / Updating Grid position" << std::endl;

					InsertGameObject(*gameObjects[i]);

					gameObjects.erase(gameObjects.begin() + i--);
				}
				else 
				{
					std::cout << "Left grid" << std::endl;

					if (parent != nullptr) 
					{
						inserted = parent->InsertGameObject(*gameObjects[i]);
					}
					else 
					{
						inserted = InsertGameObject(*gameObjects[i]);
					}

					if (inserted && !gameObjects.empty())
						gameObjects.erase(gameObjects.begin() + i--);

				}
			}
		}

		//prune any dead objects from the tree.
		int listSize = gameObjects.size();
		for (int a = 0; a < listSize; a++)
		{
			if (gameObjects[a] == nullptr)
			{
				std::cout << "Deleted nullptr" << std::endl;
				gameObjects.erase(gameObjects.begin() + a--);
				listSize--;
			}

			if (!boundaries.Contains(gameObjects[a]->transform))
			{
				std::cout << "Deleted gameObject that left grid" << std::endl;
				gameObjects.erase(gameObjects.begin() + a--);
				listSize--;
			}
		}

		// Delete empty child grids
		for (int childIndex = 0; childIndex < 8; childIndex++)
		{
			if (activeChildren[childIndex] == true)
			{
				if (childGrid[childIndex]->gameObjects.empty() && childGrid[childIndex]->activeChildCount == 0)
				{
					childGrid[childIndex] = nullptr;
					activeChildren[childIndex] = false;
					activeChildCount--;
				}
			}
		}

		// Get collisions
		for (int i = 0; i < gameObjects.size(); i++)
		{
			CollidingObjects(*gameObjects[i]);
		}
	}
}

void UniformGrid::CollidingObjects(GameObject& gameObject) {	
	// Check for collisions in this grid
	for (int i = 0; i < gameObjects.size(); i++) {
		if (&gameObject == gameObjects[i]) {
			continue;
		}

		if (gameObject.hasCollision) {
			if (BoundingBox::TestAABBOverlap(gameObject.transform, gameObjects[i]->transform))
			{
				gameObject.OnCollisionEnter(*gameObjects[i]);
				gameObjects[i]->OnCollisionEnter(gameObject);
			}
		}
	}

	// Loop through children
	// and check for collisions
	if (activeChildCount > 0) {
		for (int childIndex = 0; childIndex < 8; childIndex++) {
			if (activeChildren[childIndex] == true)
			{
				childGrid[childIndex]->CollidingObjects(gameObject);
			}
		}
	}
}

void UniformGrid::SetSize(int size)
{
	this->size = size;
}

void UniformGrid::DrawGrid() 
{
	ExtraRenderer::DrawUniformBox(boundaries);

	for (int index = 0; index < 8; index++)
	{
		if (activeChildren[index] == true)
		{
			childGrid[index]->DrawGrid();
		}
	}
}

void UniformGrid::DrawExtra()
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
		if (pendingGameObjects.size() > 0) {
			ImGui::Text("Grid Pending");
			for (int i = 0; i < pendingGameObjects.size(); i++) {
				ImGui::Text("GameObject: ");
				ImGui::SameLine();

				if (pendingGameObjects[i] != nullptr)
					ImGui::Text(pendingGameObjects[i]->transform.name.c_str());
			}
		}
	}
}