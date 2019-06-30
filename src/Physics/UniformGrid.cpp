#include "UniformGrid.h"

#include "imgui.h"

#include <iostream>

#include "Physics\Collider.h"

using namespace Graphyte;

std::map<GameObject*, std::vector<Collider*>> UniformGrid::PendingColliders; // Colliders that will be added later *TBD*
std::map<int, std::vector<int>> UniformGrid::currentlyCollidingObjects;

//std::queue<Collider*> UniformGrid::pendingColliders;			// Colliders that will be added later
std::queue<int> UniformGrid::pendingColliders;			// Colliders that will be added later
std::vector<Collider*> UniformGrid::allColliders;				// All the colliders that are in the scene

bool UniformGrid::gridReady; // FALSE by default. the tree has a few objects which need to be inserted before it is complete 

UniformGrid::UniformGrid(const Bounds & boundaries)
{
	childGrid = std::vector<UniformGrid>(8);
	this->boundaries = boundaries;
}

UniformGrid Graphyte::UniformGrid::CreateNode(const Bounds& boundary)
{
	UniformGrid ret = UniformGrid(boundary);
	ret.parent = this;
	return ret;
}

UniformGrid::UniformGrid() : boundaries(Bounds(Vector3(0, 0, 0), Vector3(0, 0, 0)))
{

}

// Public
UniformGrid::UniformGrid(const int size)
{
	std::cout << "Maximum value for float: " << std::numeric_limits<float>::max() << '\n';

	std::cout << "Grid with size: " << size  << " created\n";

	colliders.reserve(1000);

	childGrid = std::vector<UniformGrid>(8);

	this->size = size;
	int halfSize = size / 2;

	this->boundaries = Bounds(Vector3(-halfSize, -halfSize, -halfSize), Vector3(halfSize, halfSize, halfSize));
	
	this->parent = nullptr;
}

bool Graphyte::UniformGrid::isRoot() const
{
	return parent == nullptr;
}

void UniformGrid::AddCollider(Collider* col)
{

	std::cout << "Inserting " << col->gameObject->transform.name << " collider size: " << sizeof(Collider) << " bytes\n";

	allColliders.push_back(col);

	//pendingColliders.push(col);
	pendingColliders.push(allColliders.size() - 1);

	PendingColliders[col->gameObject].push_back(col);
}

bool UniformGrid::InsertCollider(int colliderPosition)
{
	// If the object doesnt fit try inserting it in the parent grid
	if (!boundaries.Contains(*allColliders[colliderPosition]->transform))
	{
		if (isRoot())
		{
			RebuildGrid();
			return false;	// Didnt fit anywhere so return false
		}
		else
		{
			std::cout << "Trying to insert into parent\n";
			return this->parent->InsertCollider(colliderPosition);
		}
	}

	// If the bounds are minimum size insert object here
	Vector3 dimensions = boundaries.max - boundaries.min;
	if (dimensions.x <= minSize && dimensions.y <= minSize && dimensions.z <= minSize)
	{
		colliders.push_back(colliderPosition);
		return true;
	}

	Vector3 half = (boundaries.max - boundaries.min) / 2.0f;
	Vector3 center = boundaries.min + half;

	Bounds childGrids[8];

	// Bottom 4
	childGrids[0] = Bounds(boundaries.min, center);		// BOTTOM LEFT																		// WORKS / TESTED
	childGrids[1] = Bounds(Vector3(center.x, boundaries.min.y, boundaries.min.z), Vector3(boundaries.max.x, center.y, center.z));	// WORKS / TESTED
	childGrids[2] = Bounds(Vector3(boundaries.min.x, boundaries.min.y, center.z), Vector3(center.x, center.y, boundaries.max.z));	// WORKS / TESTED
	childGrids[3] = Bounds(Vector3(center.x, boundaries.min.y, center.z), Vector3(boundaries.max.x, center.y, boundaries.max.z));	// WORKS / TESTED

	// Top 4
	childGrids[4] = Bounds(Vector3(boundaries.min.x, center.y, boundaries.min.z), Vector3(center.x, boundaries.max.y, center.z));	// WORKS / TESTED
	childGrids[5] = Bounds(Vector3(center.x, center.y, boundaries.min.z), Vector3(boundaries.max.x, boundaries.max.y, center.z));	// WORKS / TESTED
	childGrids[6] = Bounds(Vector3(boundaries.min.x, center.y, center.z), Vector3(center.x, boundaries.max.y, boundaries.max.z));	// WORKS / TESTED
	childGrids[7] = Bounds(center, boundaries.max);																				// WORKS / TESTED

	for (int childIndex = 0; childIndex < 8; ++childIndex)
	{
		if (childGrids[childIndex].Contains(*allColliders[colliderPosition]->transform))
		{
			//std::cout << "Inserted Into child: " << childIndex << " & Size: " << childGrids[childIndex].min.x << ", " << childGrids[childIndex].max.x << std::endl;
			childGrid[childIndex] = CreateNode(childGrids[childIndex]);
			childGrid[childIndex].parent = this;

			activeChildren[childIndex] = true;
			++activeChildCount;

			return childGrid[childIndex].InsertCollider(colliderPosition);
		}
	}

	//std::cout << "Inserted Into root: " << (boundaries.max.x - boundaries.min.x) << "\n";
	colliders.push_back(colliderPosition);
	return false;
}

void UniformGrid::RebuildGrid()
{
	auto rebuildStart = std::chrono::high_resolution_clock::now();

	childGrid = std::vector<UniformGrid>(8);
	colliders.clear();

	for (int i = 0; i < 8; i++)
	{
		activeChildren[i] = false;
	}

	activeChildCount = 0;

	// Double grid size every time a object leaves bounds
	size *= 2;

	std::cout << "Resizing Grid to: " << size * 2 << "\n";

	int halfSize = size / 2;

	this->boundaries = Bounds(Vector3(-halfSize, -halfSize, -halfSize), Vector3(halfSize, halfSize, halfSize));

	for (int i = 0; i < allColliders.size(); ++i)
	{
		InsertCollider(i);
	}

	gridReady = true;

	auto rebuildEnd = std::chrono::high_resolution_clock::now();

	std::cout << "Time elapsed to rebuild UniformGrid: " << std::chrono::duration_cast<std::chrono::nanoseconds>(rebuildEnd - rebuildStart).count() << " ns\n" ;
}

void UniformGrid::Update() {
	if (gridReady)
	{
		while (!pendingColliders.empty()) {
			InsertCollider(pendingColliders.front());
			pendingColliders.pop();
		}

		// recursively update any child grids
		for (int childIndex = 0; childIndex < 8; childIndex++)
		{
			if (activeChildren[childIndex] == true)
			{
				childGrid[childIndex].Update();
			}
		}

		// Loop through all gameObjects
		// And then loop through all their colliders

		// For each GameObject
		/*
		for (auto const&[gameObject, colliders] : PendingColliders)
		{
			// For each collider
			if (gameObject->enabled) {
				for (int i = 0; i < colliders.size(); ++i) {
					//collider[i];
				}
			}
		}
		*/

		for (int i = 0; i < colliders.size(); ++i)
		{
			if (allColliders[colliders[i]]->transform->positionHasChanged() && IsInsideWorldBounds(*allColliders[colliders[i]]->transform))
			{
				bool inserted = false;

				if (boundaries.Contains(*allColliders[colliders[i]]->transform))
				{
					InsertCollider(colliders[i]);

					colliders.erase(colliders.begin() + i--);
				}
				else
				{
					std::cout << "Left grid" << std::endl;

					if (parent != nullptr)
					{
						inserted = parent->InsertCollider(colliders[i]);
					}
					else
					{
						inserted = InsertCollider(colliders[i]);
					}

					if (inserted && !colliders.empty())
						colliders.erase(colliders.begin() + i--);

				}
			}
		}

		//prune any dead objects from the tree.
		int listSize = colliders.size();
		for (int a = 0; a < listSize; a++)
		{
			if (allColliders[colliders[a]] == nullptr)
			{
				std::cout << "Deleted nullptr" << std::endl;
				colliders.erase(colliders.begin() + a--);
				listSize--;
			}

			if (!boundaries.Contains(*allColliders[colliders[a]]->transform))
			{
				std::cout << "Deleted gameObject that left grid" << std::endl;
				colliders.erase(colliders.begin() + a--);
				listSize--;
			}
		}

		// Delete empty child grids
		for (int childIndex = 0; childIndex < 8; childIndex++)
		{
			if (activeChildren[childIndex] == true)
			{
				if (childGrid[childIndex].colliders.empty() && childGrid[childIndex].activeChildCount == 0)
				{
					activeChildren[childIndex] = false;
					activeChildCount--;
				}
			}
		}

		// Get collisions
		//std::cout << "Checking collisions\n";
		CheckCollisions();
	}
}

void UniformGrid::CheckCollisions() 
{
	// Get collisions
	for (int i = 0; i < colliders.size(); ++i)
	{
		CollidingObjects(colliders[i]);
	}
}

void UniformGrid::CollidingObjects(const int colliderIndex)
{
	Collider& colliderA = *allColliders[colliderIndex];
	const AxisAlignedBoundingBox& boundingBoxA = colliderA.transform->boundingBox;

	std::vector<int> touchingObjects = currentlyCollidingObjects[colliderIndex];
	// Check for new collisions
	for (int i = 0; i < colliders.size(); ++i)
	{
		if (colliders[i] == colliderIndex) 
		{
			continue;
		}

		bool foundCollision = false;
		int collisionIndex;
		for (int j = 0; j < touchingObjects.size(); ++j) 
		{
			if (colliders[i] == touchingObjects[j]) 
			{
				foundCollision = true;
				collisionIndex = j;
				break;
			}
		}

		Collider& colliderB = *allColliders[colliders[i]];

		if (!foundCollision) {
			if (colliderB.hasCollision)
			{
				const AxisAlignedBoundingBox& boundingBoxB = colliderB.transform->boundingBox;
				if (AxisAlignedBoundingBox::TestOverlap(boundingBoxA, boundingBoxB))
				{
					std::cout << "Collision Enter in UniformGrid\n";

					colliderA.CheckCollision(colliderB);
					colliderB.CheckCollision(colliderA);

					//colliderA.gameObject->OnCollisionEnter(colliderB);
					//colliderB.gameObject->OnCollisionEnter(colliderA);

					currentlyCollidingObjects[colliderIndex].push_back(colliders[i]);
					currentlyCollidingObjects[colliders[i]].push_back(colliderIndex);
				}
			}
		}
		else { 
			// Check if exited or still staying
			if (colliderB.hasCollision)
			{
				const AxisAlignedBoundingBox& boundingBoxB = colliderB.transform->boundingBox;
				if (AxisAlignedBoundingBox::TestOverlap(boundingBoxA, boundingBoxB))
				{
					//std::cout << "Collision Stay in UniformGrid " << i << "\n";

					//colliderA.gameObject->OnCollisionStay(colliderB);
					//colliderB.gameObject->OnCollisionStay(colliderA);

					colliderA.CheckCollision(colliderB);
					colliderB.CheckCollision(colliderA);
				}
				else {
					std::cout << "Collision Exit in UniformGrid\n";

					colliderA.gameObject->OnCollisionExit(colliderB);
					colliderB.gameObject->OnCollisionExit(colliderA);

					currentlyCollidingObjects[colliderIndex].erase(std::remove(currentlyCollidingObjects[colliderIndex].begin(), currentlyCollidingObjects[colliderIndex].end(), colliders[i]), currentlyCollidingObjects[colliderIndex].end());
					currentlyCollidingObjects[colliders[i]].erase(std::remove(currentlyCollidingObjects[colliders[i]].begin(), currentlyCollidingObjects[colliders[i]].end(), colliderIndex), currentlyCollidingObjects[colliders[i]].end());
				}
			}
		}
	}

	// Loop through children
	// and check for collisions
	if (activeChildCount > 0)
	{
		for (int childIndex = 0; childIndex < 8; ++childIndex)
		{
			if (activeChildren[childIndex] == true)
			{
				childGrid[childIndex].CollidingObjects(colliderIndex);
			}
		}
	}
}

/*
std::vector<Renderer*> Graphyte::UniformGrid::GetObjectsToRender(Camera & camera)
{
}
*/

bool UniformGrid::IsInsideWorldBounds(const Transform& transform) const
{
	if (maxSize > transform.position.x + transform.boundingBox.halfSize.x &&
		maxSize > transform.position.y + transform.boundingBox.halfSize.y &&
		maxSize > transform.position.z + transform.boundingBox.halfSize.z
		)
	{
		return true;
	}

	return false;
}

void UniformGrid::DrawGrid() const
{
	ExtraRenderer::DrawBounds(boundaries);

	for (int index = 0; index < 8; ++index)
	{
		if (activeChildren[index] == true)
		{
			childGrid[index].DrawGrid();
		}
	}
}

void UniformGrid::DrawExtra() const
{
	for (int index = 0; index < 8; ++index)
	{
		if (activeChildren[index] == true)
		{
			childGrid[index].DrawExtra();
		}
	}

	if (colliders.size() > 0) {
		ImGui::Text("Grid");
		ImGui::SameLine();
		int size = boundaries.max.x - boundaries.min.x;
		ImGui::Text(std::to_string(size).c_str());
		ImGui::SameLine();
		ImGui::Text("Contains");
		for (int i = 0; i < colliders.size(); i++) {
			ImGui::Text("GameObject: ");
			ImGui::SameLine();

			if (allColliders[colliders[i]] != nullptr)
				ImGui::Text(allColliders[colliders[i]]->transform->name.c_str());
		}
	}

	if (isRoot()) {
		bool pending = !pendingColliders.empty();
		ImGui::Checkbox("Grid Colliders Pending", &pending);

		int pendingSize = pendingColliders.size();
		ImGui::InputInt("Grid Pending Collider Count", &pendingSize);

		int allCollidersSize = allColliders.size();
		ImGui::InputInt("Collider Count", &allCollidersSize);
	}
}