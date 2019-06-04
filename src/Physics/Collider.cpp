#include "Collider.h"

#include "Rendering\MeshRenderer.h"

#include "Quickhull\QuickHull.hpp"

using namespace Graphyte;

void Graphyte::Collider::OnComponentAdded()
{
	mesh = &gameObject->GetComponent<MeshRenderer>().mesh;

	CreateConvex();
}

Vector3 Graphyte::Collider::ClosestPoint(const Vector3 & position)
{
	return Vector3();
}

void Graphyte::Collider::CreateConvex()
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

	std::cout << "Vertices in original collider: " << mesh->vertices.size() << std::endl;
	std::cout << "Vertices in convex collider: " << points.size() << std::endl;
}

void Collider::DrawCollider()
{
	MeshRenderer meshRenderer = MeshRenderer();

	meshRenderer.transform->position = transform->position;

	meshRenderer.mesh.vertices = points;
	meshRenderer.mesh.indices = pointIndices;

	meshRenderer.OnComponentAdded();

	//meshRenderer.Update();

	meshRenderer.DrawLines();
}
