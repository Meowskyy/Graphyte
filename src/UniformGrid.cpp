#include "UniformGrid.h"

std::queue<GameObject*> UniformGrid::pendingGameObjects;		// GameObjects that will be added later

bool UniformGrid::gridReady; // FALSE by default. the tree has a few objects which need to be inserted before it is complete 
bool UniformGrid::gridBuilt; // FALSE by default. there is no pre-existing tree yet. 

// Private
UniformGrid::UniformGrid(BoundingBox size, std::vector<GameObject*> gameObjectList) {
	this->boundaries = size;
	gameObjects = gameObjectList;

	curLife = -1;

	for (int i = 0; i < 8; i++) {
		activeChildren[i] = 0;
	}
}

// Public
UniformGrid::UniformGrid() {
	boundaries = BoundingBox(Vector3(0, 0, 0), Vector3(0, 0, 0));
	curLife = -1;

	for (int i = 0; i < 8; i++) {
		activeChildren[i] = 0;
	}
}

// Public
UniformGrid::UniformGrid(int size)
{
	int halfSize = size / 2;
	boundaries = BoundingBox(Vector3(-halfSize, -halfSize, -halfSize), Vector3(halfSize, halfSize, halfSize));

	curLife = -1;

	for (int i = 0; i < 8; i++) {
		activeChildren[i] = 0;
	}
}

void UniformGrid::UpdateGrid()
{
	if (!gridBuilt)
	{
		while (pendingGameObjects.size() != 0) {
			gameObjects.push_back(pendingGameObjects.front());
			pendingGameObjects.pop();
		}

		BuildGrid();
	}
	else
	{
		while (pendingGameObjects.size() != 0)
		{
			InsertGameObject(pendingGameObjects.front());
			pendingGameObjects.pop();
		}
	}

	gridReady = true;
}

bool UniformGrid::InsertGameObject(GameObject* gameObject)
{
	std::cout << "Trying to insert into " << (boundaries.max.x - boundaries.min.x) << std::endl;

	// If its the root
	// and the gameobject does not fit in
	// double the gridsize and try again
	if (isRoot()) {

	}

	if (gameObjects.empty() && activeChildCount == 0) {
		std::cout << "Insert 1 " << (boundaries.max.x - boundaries.min.x) << std::endl;
		gameObjects.push_back(gameObject);
		return true;
	}

	// Check if min size
	// If it is put the object in this grid
	// and stop
	glm::vec3 dimensions = boundaries.max - boundaries.min;
	if (dimensions.x <= minSize && dimensions.y <= minSize && dimensions.z <= minSize)
	{
		std::cout << "Insert 2" << (boundaries.max.x - boundaries.min.x) << std::endl;
		gameObjects.push_back(gameObject);
		return true;
	}

	//The object won't fit into the current region, so it won't fit into any child regions.
	//therefore, try to push it up the tree. If we're at the root node, we need to resize the whole tree.
	if (!boundaries.Contains(gameObject->transform))
	{
		if (this->parent != nullptr) 
		{
			std::cout << "Trying to insert into parent" << std::endl;
			return this->parent->InsertGameObject(gameObject);
		}
		else 
		{
			return false;
		}
	}

	// At this point, we at least know this region can contain the object but there are child nodes. Let's try to see if the object will fit
	// within a subregion of this region.
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

	if (boundaries.Contains(gameObject->transform)) {
		// Looks like it fits in this grid
		std::cout << "Fits in: " << (boundaries.max.x - boundaries.min.x) << std::endl;

		//we will try to place the object into a child node. If we can't fit it in a child node, then we insert it into the current node object list.
		for (int a = 0; a < 8; a++)
		{
			bool found = false;

			//is the object fully contained within a quadrant?
			if (childOctantHalf[a].Contains(gameObject->transform))
			{
				if (activeChildren[a] == 1)
				{
					std::cout << "Added without nullptr" << std::endl;
					return childGrid[a]->InsertGameObject(gameObject);			//Add the item into that tree and let the child tree figure out what to do with it
				}
				else
				{
					std::cout << "Added with nullptr" << std::endl;
					childGrid[a] = CreateNode(childOctantHalf[a], gameObject);   //create a new tree node with the item
					activeChildren[a] = 1;
					activeChildCount++;
				}

				std::cout << "Fits in child: " << (childOctantHalf[a].max.x - childOctantHalf[a].min.x) << std::endl;
				std::cout << "Inserted in child: " << (childOctantHalf[a].max.x - childOctantHalf[a].min.x) << std::endl;

				found = true;
			}
			else {
				std::cout << "Does not fit in child: " << (childOctantHalf[a].max.x - childOctantHalf[a].min.x) << std::endl;
			}

			if (!found) 
			{
				std::cout << "Inserted in: " << (boundaries.max.x - boundaries.min.x) << std::endl;
				gameObjects.push_back(gameObject);
				return true;
			}
		}
	}

	// either the item lies outside of the enclosed bounding box or it is intersecting it. Either way, we need to rebuild
	// the entire tree by enlarging the containing bounding box
	// Or you assign a new parent to this and rebuild that
	return false;
}

void UniformGrid::BuildGrid() {
	//terminate the recursion if we're a leaf node
	//if (gameObjects.size() <= 1)
		//return;

	Vector3 dimensions = boundaries.max - boundaries.min;

	if (dimensions == Vector3(0, 0, 0))
	{
		//FindEnclosingCube();
		//dimensions = boundaries.max - boundaries.min;
	}

	// Check to see if the dimensions of the box are greater than the minimum dimensions
	if (dimensions.x <= minSize && dimensions.y <= minSize && dimensions.z <= minSize)
	{
		return;
	}

	Vector3 half = dimensions / 2.0f;
	Vector3 center = boundaries.min + half;

	//Create subdivided regions for each octant
	BoundingBox* octant[8];
	octant[0] = new BoundingBox(boundaries.min, center);
	octant[1] = new BoundingBox(Vector3(center.x, boundaries.min.y, boundaries.min.z), Vector3(boundaries.max.x, center.y, center.z));
	octant[2] = new BoundingBox(Vector3(center.x, boundaries.min.y, center.z), Vector3(boundaries.max.x, center.y, boundaries.max.z));
	octant[3] = new BoundingBox(Vector3(boundaries.min.x, boundaries.min.y, center.z), Vector3(center.x, center.y, boundaries.max.z));
	octant[4] = new BoundingBox(Vector3(boundaries.min.x, center.y, boundaries.max.z), Vector3(center.x, boundaries.max.y, center.z));
	octant[5] = new BoundingBox(Vector3(center.x, center.y, boundaries.max.z), Vector3(boundaries.max.x, boundaries.max.y, center.z));
	octant[6] = new BoundingBox(center, boundaries.max);
	octant[7] = new BoundingBox(Vector3(boundaries.max.x, center.y, center.z), Vector3(center.x, boundaries.max.y, boundaries.max.z));

	//This will contain all of our objects which fit within each respective octant.
	std::vector<std::vector<GameObject*>> objectList(8);

	//this list contains all of the objects which got moved down the tree and can be delisted from this node.
	std::vector<GameObject*> delist;

	for (int i = 0; i < gameObjects.size(); i++)
	{
		for (int a = 0; a < 8; a++)
		{
			if (octant[a]->Contains(gameObjects[i]->transform))
			{
				objectList[a].push_back(gameObjects[i]);
				delist.push_back(gameObjects[i]);
				break;
			}
		}
	}

	// delist every moved object from this node.
	for (int i = 0; i < delist.size(); i++) {
		gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), delist[i]), gameObjects.end());
	}

	//Create child nodes where there are items contained in the bounding region
	for (int a = 0; a < 8; a++)
	{
		if (objectList[a].size() != 0)
		{
			childGrid[a] = CreateNode(octant[a], objectList[a]);
			activeChildren[a] == 1;
			++activeChildCount;

			childGrid[a]->BuildGrid();
		}
	}

	gridBuilt = true;
	gridReady = true;
}

void UniformGrid::Update() {
	if (gridReady && gridBuilt)
	{
		//Start a count down death timer for any leaf nodes which don't have objects or children.
		//when the timer reaches zero, we delete the leaf. If the node is reused before death, we double its lifespan.
		//this gives us a "frequency" usage score and lets us avoid allocating and deallocating memory unnecessarily
		if (gameObjects.size() == 0)
		{
			if (activeChildCount == 0)
			{
				if (curLife == -1)
					curLife = maxLifeSpan;
				else if (curLife > 0)
				{
					curLife--;
				}
			}
		}
		else
		{
			if (curLife != -1)
			{
				if (maxLifeSpan <= 64)
					maxLifeSpan *= 2;
				curLife = -1;
			}
		}

		std::vector<GameObject*> movedObjects(gameObjects.size());

		// go through and update every object in the current tree node
		for (int i = 0; i < gameObjects.size(); i++)
		{
			if (gameObjects[i] != nullptr) {
				//we should figure out if an object actually moved so that we know whether we need to update this node in the tree.
				if (gameObjects[i]->transform.positionHasChanged())
				{
					movedObjects.push_back(gameObjects[i]);
				}
			}
		}

		//prune any dead objects from the tree.
		int listSize = gameObjects.size();
		for (int a = 0; a < listSize; a++)
		{
			if (gameObjects[a] == nullptr)
			{
				if (std::find(movedObjects.begin(), movedObjects.end(), gameObjects[a]) != movedObjects.end()) {
					movedObjects.erase(std::remove(movedObjects.begin(), movedObjects.end(), gameObjects[a]), movedObjects.end());
				}

				gameObjects.erase(gameObjects.begin() + a--);
				listSize--;
			}
		}


		//prune out any dead branches in the tree
		for (int index = 0; index < 8; index++)
		{
			if (activeChildren[index] == 1 && childGrid[index]->curLife == 0)
			{
				if (childGrid[index]->gameObjects.size() > 0)
				{
					//throw new Exception("Tried to delete a used branch!");
					childGrid[index]->curLife = -1;
				}
				else
				{
					childGrid[index] = nullptr;
					activeChildren[index] = 0;
					activeChildCount--;
				}
			}
		}

		// recursively update any child nodes.
		for (int index = 0; index < 8; ++index)
		{
			if (activeChildren[index] == 1)
			{
				if (childGrid[index] != nullptr)
					childGrid[index]->Update();
			}
		}

		// If an object moved, we can insert it into the parent and that will insert it into the correct tree node.
		// note that we have to do this last so that we don't accidentally update the same object more than once per frame.
		for (int i = 0; i < movedObjects.size(); ++i)
		{
			if (movedObjects[i] == nullptr) {
				continue;
			}

			// figure out how far up the tree we need to go to reinsert our moved object
			// we are either using a bounding rect or a bounding sphere
			// try to move the object into an enclosing parent node until we've got full containment
			UniformGrid* current = this;

			while (!current->boundaries.Contains(movedObjects[i]->transform)) 
			{
				if (current->parent != nullptr)
				{
					current = current->parent;
				}
				else
				{
					break; //prevent infinite loops when we go out of bounds of the root node region
				}
			}


			// now, remove the object from the current node and insert it into the current containing node.
			gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), movedObjects[i]), gameObjects.end());
			current->InsertGameObject(movedObjects[i]);   //this will try to insert the object as deep into the tree as we can go.
		}

		/*
		//now that all objects have moved and they've been placed into their correct nodes in the octree, we can look for collisions.
		if (isRoot())
		{
			//This will recursively gather up all collisions and create a list of them.
			//this is simply a matter of comparing all objects in the current root node with all objects in all child nodes.
			//note: we can assume that every collision will only be between objects which have moved.
			//note 2: An explosion can be centered on a point but grow in size over time. In this case, you'll have to override the update method for the explosion.
			std::vector<IntersectionRecord> irList = GetIntersection(new std::vector<GameObject>());

			foreach(IntersectionRecord ir in irList)
			{
				if (ir.PhysicalObject != null)
					ir.PhysicalObject.HandleIntersection(ir);
				if (ir.OtherPhysicalObject != null)
					ir.OtherPhysicalObject.HandleIntersection(ir);
			}
		}
		*/
	} //end if tree built
	else
	{
		if (pendingGameObjects.size() > 0)
		{
			//ProcessPendingItems();
			//Update(time);   //try this again...
		}
	}
}