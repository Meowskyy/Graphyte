#include "UniformGrid.h"

std::vector<GameObject*> UniformGrid::pendingGameObjects;		// GameObjects that will be added later
//std::vector<GameObject*> UniformGrid::collidingGameObjects;
std::vector<std::string> UniformGrid::collidingGameObjects;

bool UniformGrid::gridReady; // FALSE by default. the tree has a few objects which need to be inserted before it is complete 
float UniformGrid::size;

// Private
UniformGrid::UniformGrid(BoundingBox size, std::vector<GameObject*> gameObjectList) 
{
	this->boundaries = size;
	gameObjects = gameObjectList;

	curLife = -1;

	for (int i = 0; i < 8; i++) {
		activeChildren[i] = 0;
	}
}

// Public
UniformGrid::UniformGrid() 
{
	boundaries = BoundingBox(Vector3(0, 0, 0), Vector3(0, 0, 0));
	curLife = -1;
}

// Public
UniformGrid::UniformGrid(int size)
{
	int halfSize = size / 2;
	this->boundaries = BoundingBox(Vector3(-halfSize, -halfSize, -halfSize), Vector3(halfSize, halfSize, halfSize));
	this->parent = nullptr;

	curLife = -1;
}

bool UniformGrid::InsertGameObject(GameObject* gameObject)
{

	// If the object doesnt fit try inserting it in the parent grid
	if (!boundaries.Contains(gameObject->transform))
	{
		if (this->parent != nullptr)
		{
			std::cout << "Trying to insert into parent" << std::endl;
			return this->parent->InsertGameObject(gameObject);
		}
		else
		{
			RebuildGrid();
			return true;
		}
	}


	Vector3 dimensions = boundaries.max - boundaries.min;
	if (dimensions.x <= minSize && dimensions.y <= minSize && dimensions.z <= minSize)
	{
		//std::cout << "Inserted because dimensions: " << (boundaries.max.x - boundaries.min.x) << std::endl;
		gameObjects.push_back(gameObject);
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
		if (childGrids[childIndex].Contains(gameObject->transform))
		{
			activeChildren[childIndex] = true;

			//std::cout << "Inserted Into child: " << childIndex << " & Size: " << childGrids[childIndex].min.x << ", " << childGrids[childIndex].max.x << std::endl;
			childGrid[childIndex] = CreateNode(childGrids[childIndex]);
			childGrid[childIndex]->parent = this;

			activeChildCount++;

			return childGrid[childIndex]->InsertGameObject(gameObject);
		}
	}

	std::cout << "Does not fit in any child" << std::endl;

	//std::cout << "Inserted Into root: " << (boundaries.max.x - boundaries.min.x) << std::endl;
	gameObjects.push_back(gameObject);

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

	for (int i = 0; i < pendingGameObjects.size(); i++) 
	{
		InsertGameObject(pendingGameObjects[i]);
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

		std::vector<GameObject*> movedGameObjects;
		for (int i = 0; i < gameObjects.size(); i++)
		{
			if (gameObjects[i]->transform.positionHasChanged())
			{	
				if (boundaries.Contains(gameObjects[i]->transform)) {
					std::cout << "Still fits inside" << std::endl;

					movedGameObjects.push_back(gameObjects[i]);

					InsertGameObject(gameObjects[i]);
					gameObjects.erase(gameObjects.begin() + i);
					i--;
				}
				else 
				{
					std::cout << "Left grid" << std::endl;
					movedGameObjects.push_back(gameObjects[i]);

					if (parent != nullptr) 
					{
						parent->InsertGameObject(gameObjects[i]);
					}
					else 
					{
						InsertGameObject(gameObjects[i]);
					}

					if (gameObjects.empty()) {
						break;
					}

					// Delete object from this tree since it should be in the parent now
					// And reduce i by 1 because of deletion
					gameObjects.erase(gameObjects.begin() + i);
					i--;
				}
			}
		}

		//prune any dead objects from the tree.
		int listSize = gameObjects.size();
		for (int a = 0; a < listSize; a++)
		{
			if (gameObjects[a] == nullptr)
			{
				// gameObjects.erase(gameObjects.begin() + a--);
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


		collidingGameObjects.clear();
		//for (int i = 0; i < movedGameObjects.size(); i++) 
		//{
		for (int i = 0; i < gameObjects.size(); i++)
		{
			//for (int gameObjectIndex = 0; gameObjectIndex < gameObjects.size(); gameObjectIndex++)
			for (int gameObjectIndex = i; gameObjectIndex < gameObjects.size(); gameObjectIndex++)
			{		
				if (i == gameObjectIndex) 
				{
					continue;
				}

				if (BoundingBox::TestAABBOverlap(&gameObjects[i]->transform, &gameObjects[gameObjectIndex]->transform))
				//if (BoundingBox::TestAABBOverlap(&movedGameObjects[i]->transform, &gameObjects[gameObjectIndex]->transform))
				{
					//collidingGameObjects.push_back(movedGameObjects[i]);
					collidingGameObjects.push_back(gameObjects[i]->transform.name + " with " + gameObjects[gameObjectIndex]->transform.name);

					//gameObjects[i]->collisionList.push_back(gameObjects[gameObjectIndex]);
					//gameObjects[gameObjectIndex]->collisionList.push_back(gameObjects[i]);
				}
			}
		}

	}
	else
	{
		if (pendingGameObjects.size() > 0)
		{
			// ProcessPendingItems();
			// Update(time);   //try this again...
		}
	}
}

void UniformGrid::SetSize(int size)
{
	this->size = size;
}
