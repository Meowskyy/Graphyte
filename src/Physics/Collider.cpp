#include "Collider.h"

#include "Rendering\MeshRenderer.h"

#include "Quickhull\QuickHull.hpp"

#include <Physics\UniformGrid.h>

using namespace Graphyte;

void Collider::OnComponentAdded()
{
	std::cout << "Added to name: " << transform->name << "\n";

	mesh = &gameObject->GetComponent<MeshRenderer>().mesh;

	gameObject->GetComponent<MeshRenderer>().col = this;

	//CreateConvex();

	UniformGrid::AddCollider(this);
}

Vector3 Collider::ClosestPoint(const Vector3 & position)
{
	return Vector3();
}

bool Collider::CheckForTriangleIntersection(const Triangle& triangleA, const Triangle& triangleB)
{
	return false;
}

void Collider::CreateConvex()
{
	quickhull::QuickHull<float> qh; // Could be double as well
	std::vector<quickhull::Vector3<float>> pointCloud;

	for (auto& vertex : mesh->vertices) {
		pointCloud.push_back(quickhull::Vector3<float>(vertex.x, vertex.y, vertex.z));
	}

	auto hull = qh.getConvexHull(pointCloud, true, false);
	auto indexBuffer = hull.getIndexBuffer();
	auto vertexBuffer = hull.getVertexBuffer();
	// Do what you want with the convex triangle mesh

	for (int i = 0; i < vertexBuffer.size(); i++)
	{
		Vector3 vert = Vector3(vertexBuffer[i].x, vertexBuffer[i].y, vertexBuffer[i].z);
		points.push_back(vert);
	}

	for (int i = 0; i < indexBuffer.size(); i++)
	{
		pointIndices.push_back(indexBuffer[i]);
	}

	std::cout << "Name: " << transform->name << "\n";
	std::cout << "Vertices in original collider: " << mesh->vertices.size() << std::endl;
	
	std::cout << "Vertices in convex collider: " << points.size() << std::endl;
	std::cout << "Indices in convex collider: " << pointIndices.size() << std::endl;
}

void Collider::DrawCollider()
{
	std::cout << "Drawing collider Name: " << transform->name << "\n";

	MeshRenderer meshRenderer = MeshRenderer();

	meshRenderer.transform->position = transform->position;
	meshRenderer.mesh.vertices = this->points;
	meshRenderer.mesh.indices = this->pointIndices;

	meshRenderer.OnComponentAdded();
	meshRenderer.mesh.SetupMesh();

	meshRenderer.DrawMesh();
}
