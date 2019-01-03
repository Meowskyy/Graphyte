#pragma once

#include "BoundingBox.h"
#include "Objects\GameObject.h"
#include "ExtraRenderer.h"

#include <vector>
#include <queue>

const int minSize = 16;

class UniformGrid {
private:
	UniformGrid(BoundingBox size, std::vector<GameObject*> gameObjectList) {
		this->boundaries = size;
		gameObjects = gameObjectList;

		isReady = true;
		// m_curLife = -1;
	}
public:
	BoundingBox boundaries;
	UniformGrid* parent;

	bool isReady = false;
	bool hasChildren = false;

	// Child trees
	UniformGrid* childGrid[8];
	//std::vector<UniformGrid*> childGrid;
	char activeChildren = 0; // Bitmask for checking which children are in use

	std::vector<GameObject*> gameObjects;					// GameObjects that this grid contains
	//static std::queue<GameObject*> pendingGameObjects;	// GameObjects that will be added later

	UniformGrid() {}

	bool isRoot() {
		// Just check if it doesnt have a parent
		// Might be a bad idea, as you could manually assign it to be the root node somehow
		return (parent == nullptr);
	}

	bool InsertGameObject(GameObject* gameObject)
	{
		// If its the root
		// Double the gridsize and check again
		if (isRoot()) {

		}

		// If the gameObject fits in this part
		if (CanFitIntoBoundaries(gameObject)) {

			/*
			// Check if it fits in any childGrid
			bool found = false;
			//we will try to place the object into a child node. If we can't fit it in a child node, then we insert it into the current node object list.
			for (int a = 0; a < 8; a++)
			{
				//is the object fully contained within a quadrant?
				if (childOctantHalf[a]->Contains(gameObject->transform.boundingBox))
				{
					if (childGrid[a] != nullptr)
					{
						return childGrid[a]->InsertGameObject(gameObject);   //Add the item into that tree and let the child tree figure out what to do with it
					}
					else
					{
						childGrid[a] = CreateNode(childOctantHalf[a], gameObject);   //create a new tree node with the item
						activeChildren |= (char)(1 << a);
					}
					found = true;
				}
			}
			*/

			return true;
		}

		if (gameObjects.size() == 0 && activeChildren == 0)
		{
			gameObjects.push_back(gameObject);
			return true;
		}

		glm::vec3 dimensions = boundaries.max - boundaries.min;
		if (dimensions.x <= minSize && dimensions.y <= minSize && dimensions.z <= minSize)
		{
			gameObjects.push_back(gameObject);
			return true;
		}

		//The object won't fit into the current region, so it won't fit into any child regions.
		//therefore, try to push it up the tree. If we're at the root node, we need to resize the whole tree.
		if (!boundaries.Contains(gameObject->transform.boundingBox))
		{
			if (this->parent != nullptr) {
				return this->parent->InsertGameObject(gameObject);
			}
			else {
				return false;
			}
		}

		//At this point, we at least know this region can contain the object but there are child nodes. Let's try to see if the object will fit
		//within a subregion of this region.
		glm::vec3 doub = dimensions * 2.0f; // Double the dimensions for making a parent tree
		glm::vec3 half = dimensions / 2.0f; // Half the dimension for making a child tree
		glm::vec3 centerHalf = boundaries.min + half;
		glm::vec3 centerDouble = boundaries.min + doub;

		// Find or create subdivided regions for each octant in the current region
		BoundingBox childOctantHalf[8];
		childOctantHalf[0] = (childGrid[0] != nullptr) ? childGrid[0]->boundaries : BoundingBox(boundaries.min, centerHalf);
		childOctantHalf[1] = (childGrid[1] != nullptr) ? childGrid[1]->boundaries : BoundingBox(glm::vec3(centerHalf.x, boundaries.min.y, boundaries.min.z), glm::vec3(boundaries.max.x, centerHalf.y, centerHalf.z));
		childOctantHalf[2] = (childGrid[2] != nullptr) ? childGrid[2]->boundaries : BoundingBox(glm::vec3(centerHalf.x, boundaries.min.y, centerHalf.z), glm::vec3(boundaries.max.x, centerHalf.y, boundaries.max.z));
		childOctantHalf[3] = (childGrid[3] != nullptr) ? childGrid[3]->boundaries : BoundingBox(glm::vec3(boundaries.min.x, boundaries.min.y, centerHalf.z), glm::vec3(centerHalf.x, centerHalf.y, boundaries.max.z));
		childOctantHalf[4] = (childGrid[4] != nullptr) ? childGrid[4]->boundaries : BoundingBox(glm::vec3(boundaries.min.x, centerHalf.y, boundaries.min.z), glm::vec3(centerHalf.x, boundaries.max.y, centerHalf.z));
		childOctantHalf[5] = (childGrid[5] != nullptr) ? childGrid[5]->boundaries : BoundingBox(glm::vec3(centerHalf.x, centerHalf.y, boundaries.min.z), glm::vec3(boundaries.max.x, boundaries.max.y, centerHalf.z));
		childOctantHalf[6] = (childGrid[6] != nullptr) ? childGrid[6]->boundaries : BoundingBox(centerHalf, boundaries.max);
		childOctantHalf[7] = (childGrid[7] != nullptr) ? childGrid[7]->boundaries : BoundingBox(glm::vec3(boundaries.min.x, centerHalf.y, centerHalf.z), glm::vec3(centerHalf.x, boundaries.max.y, boundaries.max.z));

		// Find or create subdivided regions for each octant in the current region
		BoundingBox childOctantDouble[8];
		childOctantDouble[0] = (childGrid[0] != nullptr) ? childGrid[0]->boundaries : BoundingBox(boundaries.min, centerDouble);
		childOctantDouble[1] = (childGrid[1] != nullptr) ? childGrid[1]->boundaries : BoundingBox(glm::vec3(centerDouble.x, boundaries.min.y, boundaries.min.z), glm::vec3(boundaries.max.x, centerDouble.y, centerDouble.z));
		childOctantDouble[2] = (childGrid[2] != nullptr) ? childGrid[2]->boundaries : BoundingBox(glm::vec3(centerDouble.x, boundaries.min.y, centerDouble.z), glm::vec3(boundaries.max.x, centerDouble.y, boundaries.max.z));
		childOctantDouble[3] = (childGrid[3] != nullptr) ? childGrid[3]->boundaries : BoundingBox(glm::vec3(boundaries.min.x, boundaries.min.y, centerDouble.z), glm::vec3(centerDouble.x, centerDouble.y, boundaries.max.z));
		childOctantDouble[4] = (childGrid[4] != nullptr) ? childGrid[4]->boundaries : BoundingBox(glm::vec3(boundaries.min.x, centerDouble.y, boundaries.min.z), glm::vec3(centerDouble.x, boundaries.max.y, centerDouble.z));
		childOctantDouble[5] = (childGrid[5] != nullptr) ? childGrid[5]->boundaries : BoundingBox(glm::vec3(centerDouble.x, centerDouble.y, boundaries.min.z), glm::vec3(boundaries.max.x, boundaries.max.y, centerDouble.z));
		childOctantDouble[6] = (childGrid[6] != nullptr) ? childGrid[6]->boundaries : BoundingBox(centerDouble, boundaries.max);
		childOctantDouble[7] = (childGrid[7] != nullptr) ? childGrid[7]->boundaries : BoundingBox(glm::vec3(boundaries.min.x, centerDouble.y, centerDouble.z), glm::vec3(centerDouble.x, boundaries.max.y, boundaries.max.z));


		// First, is the item completely contained within the root bounding box?
		// note2: I shouldn't actually have to compensate for this. If an object is out of our predefined bounds, then we have a problem/error.
		// Wrong. Our initial bounding box for the terrain is constricting its height to the highest peak. Flying units will be above that.
		// Fix: I resized the enclosing box to 256x256x256. This should be sufficient.
		if (gameObject->transform.boundingBox.max != gameObject->transform.boundingBox.min && boundaries.Contains(gameObject->transform.boundingBox))
		{
			bool found = false;
			//we will try to place the object into a child node. If we can't fit it in a child node, then we insert it into the current node object list.
			for (int a = 0; a < 8; a++)
			{
				//is the object fully contained within a quadrant?
				if (childOctantHalf[a].Contains(gameObject->transform.boundingBox))
				{
					if (childGrid[a] != nullptr)
					{
						return childGrid[a]->InsertGameObject(gameObject);   //Add the item into that tree and let the child tree figure out what to do with it
					}
					else
					{
						childGrid[a] = CreateNode(childOctantHalf[a], gameObject);   //create a new tree node with the item
						activeChildren |= (char)(1 << a);
					}
					found = true;
				}
			}

			//we couldn't fit the item into a smaller box, so we'll have to insert it in this region
			if (!found)
			{
				gameObjects.push_back(gameObject);
				return true;
			}
		}

		// either the item lies outside of the enclosed bounding box or it is intersecting it. Either way, we need to rebuild
		// the entire tree by enlarging the containing bounding box
		// Or you assign a new parent to this and rebuild that
		return false;
	}

	UniformGrid* CreateNode(BoundingBox boundary, GameObject* gameObject)
	{
		std::vector<GameObject*> objList(1); // sacrifice potential CPU time for a smaller memory footprint
		objList.push_back(gameObject);
		UniformGrid* ret = new UniformGrid(boundary, objList);
		ret->parent = this;
		return ret;
	}

	bool CanFitIntoBoundaries(GameObject* gameObject) {
		glm::vec3 objMin = gameObject->transform.boundingBox.min + gameObject->transform.position;
		glm::vec3 objMax = gameObject->transform.boundingBox.max + gameObject->transform.position;

		if (boundaries.min.x < objMin.x &&
			boundaries.min.y < objMin.y &&
			boundaries.min.z < objMin.z &&
			
			boundaries.max.x > objMax.x &&
			boundaries.max.y > objMax.y &&
			boundaries.max.z > objMax.z
		) 
		{
			//std::cout << gameObject->transform.name << " can completely fit inside Grid" << std::endl;
			return true;
		}

		//std::cout << gameObject->transform.name << " can not completely fit inside Grid" << std::endl;
		return false;

		/*
		//we will try to place the object into a child node. If we can't fit it in a child node, then we insert it into the current node object list.
		for (int a = 0; a < 8; a++)
		{
			//is the object fully contained within a quadrant?
			if (childOctantHalf[a]->Contains(gameObject->transform.boundingBox))
			{
				if (childGrid[a] != nullptr)
				{
					return childGrid[a]->InsertGameObject(gameObject);   //Add the item into that tree and let the child tree figure out what to do with it
				}
				else
				{
					childGrid[a] = CreateNode(childOctantHalf[a], gameObject);   //create a new tree node with the item
					activeChildren |= (char)(1 << a);
				}
				found = true;
			}
		}
		*/ 
		// Bitmasking guide
	}

	// TODO: What should the default size be?
	void SetupGrid(int size = 256) {
		int halfSize = size / 2;
		boundaries = BoundingBox(Vector3(-halfSize, -halfSize, -halfSize), Vector3(halfSize, halfSize, halfSize));

		Vector3 dimensions = boundaries.max - boundaries.min;

		//Check to see if the dimensions of the box are greater than the minimum dimensions
		if (dimensions.x <= minSize && dimensions.y <= minSize && dimensions.z <= minSize)
		{
			return;
		}

		//Create subdivided regions for each octant
		Vector3 half = dimensions / 2.0f;
		Vector3 center = boundaries.min + half;

		BoundingBox octant[8];
		octant[0] = BoundingBox(boundaries.min, center);
		octant[1] = BoundingBox(Vector3(center.x, boundaries.min.y, boundaries.min.z), Vector3(boundaries.max.x, center.y, center.z));
		octant[2] = BoundingBox(Vector3(center.x, boundaries.min.x, center.z), Vector3(boundaries.max.x, center.y, boundaries.max.z));
		octant[3] = BoundingBox(Vector3(boundaries.min.x, boundaries.min.y, center.z), Vector3(center.x, center.y, boundaries.max.z));
		octant[4] = BoundingBox(Vector3(boundaries.min.x, center.y, boundaries.min.z), Vector3(center.x, boundaries.max.y, center.z));
		octant[5] = BoundingBox(Vector3(center.x, center.y, boundaries.min.z), Vector3(boundaries.max.x, boundaries.max.y, center.z));
		octant[6] = BoundingBox(center, boundaries.max);
		octant[7] = BoundingBox(Vector3(boundaries.min.x, center.y, center.z), Vector3(center.x, boundaries.max.y, boundaries.max.z));

		for (int a = 0; a < 8; a++)
		{
			GameObject* gameObject;
			std::vector<GameObject*> objList(1); // sacrifice potential CPU time for a smaller memory footprint
			objList.push_back(gameObject);

			childGrid[a] = CreateNode(octant[a], gameObject);
			childGrid[a]->SetupChild();
			
			activeChildren |= (char)(1 << a);

			hasChildren = true;
		}
	}

	void SetupChild() {
		Vector3 dimensions = boundaries.max - boundaries.min;

		//Check to see if the dimensions of the box are greater than the minimum dimensions
		if (dimensions.x <= minSize && dimensions.y <= minSize && dimensions.z <= minSize)
		{
			return;
		}

		//Create subdivided regions for each octant
		Vector3 half = dimensions / 2.0f;
		Vector3 center = boundaries.min + half;

		BoundingBox octant[8];
		octant[0] = BoundingBox(boundaries.min, center);
		octant[1] = BoundingBox(Vector3(center.x, boundaries.min.y, boundaries.min.z), Vector3(boundaries.max.x, center.y, center.z));
		octant[2] = BoundingBox(Vector3(center.x, boundaries.min.x, center.z), Vector3(boundaries.max.x, center.y, boundaries.max.z));
		octant[3] = BoundingBox(Vector3(boundaries.min.x, boundaries.min.y, center.z), Vector3(center.x, center.y, boundaries.max.z));
		octant[4] = BoundingBox(Vector3(boundaries.min.x, center.y, boundaries.min.z), Vector3(center.x, boundaries.max.y, center.z));
		octant[5] = BoundingBox(Vector3(center.x, center.y, boundaries.min.z), Vector3(boundaries.max.x, boundaries.max.y, center.z));
		octant[6] = BoundingBox(center, boundaries.max);
		octant[7] = BoundingBox(Vector3(boundaries.min.x, center.y, center.z), Vector3(center.x, boundaries.max.y, boundaries.max.z));

		for (int a = 0; a < 8; a++)
		{
			GameObject* gameObject;
			std::vector<GameObject*> objList(1); // sacrifice potential CPU time for a smaller memory footprint
			objList.push_back(gameObject);

			childGrid[a] = CreateNode(octant[a], gameObject);
			childGrid[a]->SetupChild();

			//std::cout << "Created Node " << std::endl;

			activeChildren |= (char)(1 << a);

			hasChildren = true;
		}
	}

	void DrawGrid() {
		//ExtraRenderer::DrawAABB(boundaries, Vector3(0, 0, 0));

		// Drawing the bounding boxes of the child grids
		if (hasChildren) {
			for (int a = 0; a < 8; a++)
			{
				childGrid[a]->DrawGrid();
			}
		}
	}
};