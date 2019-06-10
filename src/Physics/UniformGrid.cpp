#include "UniformGrid.h"

#include "imgui.h"

#include <iostream>

#include "Physics\Collider.h"

using namespace Graphyte;

std::vector<std::string> UniformGrid::collidingGameObjects;

std::map<GameObject*, std::vector<Collider*>> UniformGrid::PendingColliders; // Colliders that will be added later *TBD*

std::queue<Collider*> UniformGrid::pendingColliders;			// Colliders that will be added later
std::vector<Collider*> UniformGrid::allColliders;				// All the colliders that are in the scene

bool UniformGrid::gridReady; // FALSE by default. the tree has a few objects which need to be inserted before it is complete 

UniformGrid::UniformGrid(const Bounds & boundaries)
{
	childGrid = std::vector<UniformGrid>(8);
	this->boundaries = boundaries;
}

UniformGrid Graphyte::UniformGrid::CreateNode(const Bounds & boundary)
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

	std::cout << "Inserting " << col->gameObject->transform.name << "\n";

	allColliders.push_back(col);

	pendingColliders.push(col);

	PendingColliders[col->gameObject].push_back(col);

	//return root->InsertCollider(*col);
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
		InsertCollider(*allColliders[i]);
	}

	gridReady = true;

	auto rebuildEnd = std::chrono::high_resolution_clock::now();

	std::cout << "Time elapsed to rebuild UniformGrid: " << std::chrono::duration_cast<std::chrono::nanoseconds>(rebuildEnd - rebuildStart).count() << " ns\n" ;
}

void UniformGrid::Update() {
	if (gridReady)
	{
		while (!pendingColliders.empty()) {
			InsertCollider(*pendingColliders.front());
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
		for (auto const&[gameObject, colliders] : PendingColliders)
		{
			// For each collider
			if (gameObject->enabled) {
				for (int i = 0; i < colliders.size(); ++i) {
					//collider[i];
				}
			}
		}

		for (int i = 0; i < colliders.size(); ++i)
		{
			if (colliders[i]->transform->positionHasChanged() && IsInsideBounds(*colliders[i]->transform))
			{
				bool inserted = false;

				if (boundaries.Contains(*colliders[i]->transform))
				{
					InsertCollider(*colliders[i]);

					colliders.erase(colliders.begin() + i--);
				}
				else
				{
					std::cout << "Left grid" << std::endl;

					if (parent != nullptr)
					{
						inserted = parent->InsertCollider(*colliders[i]);
					}
					else
					{
						inserted = InsertCollider(*colliders[i]);
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
			if (colliders[a] == nullptr)
			{
				std::cout << "Deleted nullptr" << std::endl;
				colliders.erase(colliders.begin() + a--);
				listSize--;
			}

			if (!boundaries.Contains(*colliders[a]->transform))
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
		for (int i = 0; i < colliders.size(); i++)
		{
			CollidingObjects(*colliders[i], i);
		}
	}
}

bool UniformGrid::InsertCollider(Collider& collider)
{
	// If the object doesnt fit try inserting it in the parent grid
	if (!boundaries.Contains(*collider.transform))
	{
		if (isRoot())
		{
			RebuildGrid();
			return false;	// Didnt fit anywhere so return false
		}
		else
		{
			std::cout << "Trying to insert into parent\n";
			return this->parent->InsertCollider(collider);
		}
	}

	// If the bounds are minimum size insert object here
	Vector3 dimensions = boundaries.max - boundaries.min;
	if (dimensions.x <= minSize && dimensions.y <= minSize && dimensions.z <= minSize)
	{
		colliders.push_back(&collider);
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
		if (childGrids[childIndex].Contains(*collider.transform))
		{
			//std::cout << "Inserted Into child: " << childIndex << " & Size: " << childGrids[childIndex].min.x << ", " << childGrids[childIndex].max.x << std::endl;
			childGrid[childIndex] = CreateNode(childGrids[childIndex]);
			childGrid[childIndex].parent = this;

			activeChildren[childIndex] = true;
			++activeChildCount;

			return childGrid[childIndex].InsertCollider(collider);
		}
	}

	//std::cout << "Inserted Into root: " << (boundaries.max.x - boundaries.min.x) << "\n";
	colliders.push_back(&collider);
	return false;
}

void UniformGrid::CollidingObjects(Collider& collider) const
{
	// Check for collisions in this grid
	for (int i = 0; i < colliders.size(); ++i) 
	{
		if (&collider == colliders[i]) 
		{
			continue;
		}

		if (colliders[i]->hasCollision) 
		{
			if (AxisAlignedBoundingBox::TestOverlap(collider.transform->boundingBox, colliders[i]->transform->boundingBox))
			{
				collider.gameObject->OnCollisionEnter(*colliders[i]);
				colliders[i]->gameObject->OnCollisionEnter(collider);
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
				childGrid[childIndex].CollidingObjects(collider);
			}
		}
	}
}

void UniformGrid::CollidingObjects(Collider& collider, const int index) const
{	
	// Check for collisions in this grid
	for (int i = index; i < colliders.size(); ++i)
	{
		if (&collider == colliders[i]) 
		{
			continue;
		}

		if (colliders[i]->hasCollision) 
		{
			if (AxisAlignedBoundingBox::TestOverlap(collider.transform->boundingBox, colliders[i]->transform->boundingBox))
			{
				collider.gameObject->OnCollisionEnter(*colliders[i]);
				colliders[i]->gameObject->OnCollisionEnter(collider);
			}
		}
	}

	// Loop through children
	// and check for collisions
	if (activeChildCount > 0) 
	{
		for (int childIndex = 0; childIndex < 8; childIndex++) 
		{
			if (activeChildren[childIndex] == true)
			{
				childGrid[childIndex].CollidingObjects(collider);
			}
		}
	}
}

bool UniformGrid::IsInsideBounds(const Transform & transform) const
{
	if (maxSize > transform.position.x + transform.boundingBox.max.x &&
		maxSize > transform.position.y + transform.boundingBox.max.y &&
		maxSize > transform.position.z + transform.boundingBox.max.z
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

			if (colliders[i] != nullptr)
				ImGui::Text(colliders[i]->transform->name.c_str());
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