#include "Collider.h"

#include "Rendering\MeshRenderer.h"

#include "Quickhull\QuickHull.hpp"

#include <Physics\UniformGrid.h>

#include "Rigidbody.h"

using namespace Graphyte;

// Construct from triangle:
Plane::Plane(const Vector3& p1, const Vector3& p2, const Vector3& p3)
{
	normal = glm::cross(p2 - p1, p3 - p1);
	normal = glm::normalize(normal);
	origin = p1;
	equation[0] = normal.x;
	equation[1] = normal.y;
	equation[2] = normal.z;
	equation[3] = -(normal.x * origin.x + normal.y * origin.y + normal.z * origin.z);
}

bool Plane::isFrontFacingTo(const Vector3& direction) const {
	double dot = glm::dot(normal, direction);
	return (dot <= 0);
}
double Plane::signedDistanceTo(const Vector3& point) const {
	return (glm::dot(point, normal)) + equation[3];
}

void Collider::OnComponentAdded()
{
	std::cout << "Added to name: " << transform->name << "\n";

	mesh = &gameObject->GetComponent<MeshRenderer>().mesh;

	gameObject->GetComponent<MeshRenderer>().col = this;

	std::cout << "Mesh Renderer Vertices: " << mesh->vertices.size() << "\n";
	if (mesh->indices.size() > 0) 
	{
		for (int i = 0; i < mesh->indices.size(); i += 3) 
		{
			int indexA = mesh->indices[i];
			int indexB = mesh->indices[i + 1];
			int indexC = mesh->indices[i + 2];

			points.push_back(mesh->vertices[indexA]);
			points.push_back(mesh->vertices[indexB]);
			points.push_back(mesh->vertices[indexC]);

			Plane plane = Plane(mesh->vertices[indexA], mesh->vertices[indexB], mesh->vertices[indexC]);
			planes.push_back(plane);
		}
	}

	std::cout << "PlaneCount: " << planes.size() << "\n";

	rigidbody = &gameObject->GetComponent<Rigidbody>();

	//CreateConvex();

	UniformGrid::AddCollider(this);
}

Vector3 Collider::ClosestPoint(const Vector3& position) const
{
	return Vector3();
}

bool Collider::SphereTriCollisionDetect(const Vector3 position, const Plane& plane) 
{
	Vector3 N = plane.normal;
	Vector3 velocity = rigidbody->velocity;

	float SignedDistance = plane.signedDistanceTo(position);

	//float t1 = (-1.0f - SignedDistance) / (N * velocity);

	return true;
}

bool Collider::TriTriCollisionDetect(const Triangle& triangle1, const Triangle& triangle2) const
{
	//Detect Collision
	//Triangle's vertices V1, V2, V3
	//Get mesh1's triangle
	Vector3 tri1V1 = triangle1.v0;
	Vector3 tri1V2 = triangle1.v1;
	Vector3 tri1V3 = triangle1.v2;

	//Find the normal using U, V coordinates
	Vector3 U = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 V = Vector3(0.0f, 0.0f, 0.0f);
	Vector3 faceNormal = Vector3(0.0f, 0.0f, 0.0f);

	U = tri1V2 - tri1V1;
	V = tri1V3 - tri1V1;

	//Comput face normal using U, V
	faceNormal = glm::normalize(glm::cross(U, V));

	// ALL RIGHT TILL THIS POINT

	//Calculate a point on the triangle
	Vector3 triPoint = (tri1V1 + tri1V2 + tri1V3) / 3.0f;

	//Get plane equation "Ax + By + Cz + D = 0" Variables
	float tri1A = faceNormal.x;
	float tri1B = faceNormal.y;
	float tri1C = faceNormal.z;
	float tri1D = (-tri1A * triPoint.x - tri1B * triPoint.y - tri1C * triPoint.z);

	//Second Triangle's vertices V1, V2, V3
	Vector3 tri2V1 = triangle2.v0;
	Vector3 tri2V2 = triangle2.v1;
	Vector3 tri2V3 = triangle2.v2;

	float ep1, ep2, t = 0.0f;
	float planeIntersectX, planeIntersectY, planeIntersectZ = 0.0f;
	Vector3 pointInPlane = Vector3(0.0f, 0.0f, 0.0f);

	Vector3 t2V[4]; //Triangle 2 vertices, array used for loop
	t2V[0] = tri2V1;
	t2V[1] = tri2V2;
	t2V[2] = tri2V3;
	t2V[3] = tri2V1;

	for (int k = 0; k < 3; k++)
	{
		//Calculate where on the line, created from this trangles
		//edge (eg. tri2V1, tri2V2), intersects with the plane
		ep1 = (tri1A * t2V[k].x) + (tri1B * t2V[k].y) + (tri1C * t2V[k].z);
		ep2 = (tri1A * t2V[k + 1].x) + (tri1B * t2V[k + 1].y) + (tri1C * t2V[k + 1].z);

		//Set t to -1 in case there is a divide-by-zero
		t = -1;

		//Make sure there are no divide-by-zeros
		if (ep1 != ep2)
			t = -(ep2 + tri1D) / (ep1 - ep2);

		//If the lines intersection with the triangles plane was between the
		//two vertices (tri2V1, tri2V2), calculate the point of intersection
		if (t >= 0.0f && t <= 1.0f)
		{
			planeIntersectX = (t2V[k].x * t) + (t2V[k + 1].x * (1 - t));
			planeIntersectY = (t2V[k].y * t) + (t2V[k + 1].y * (1 - t));
			planeIntersectZ = (t2V[k].z * t) + (t2V[k + 1].z * (1 - t));

			pointInPlane = Vector3(planeIntersectX, planeIntersectY, planeIntersectZ);

			//Call function to check if point is in the triangle
			if (PointInTriangle(tri1V1, tri1V2, tri1V3, pointInPlane))
			{
				return true; //There was an intersection
			}
		}
	}

	return false; //There was no intersection
}

bool Collider::PointInTriangle(const Vector3 triV1, const Vector3 triV2, const Vector3 triV3, const Vector3 point) const
{
	//To find out if the point is inside the triangle, we will first find the area
	//of the entire triangle. After we find the area of the entire triangle, we will
	//use the point as a vertex, and create 3 more triangles using the three vertices
	//which make up the first triangle. We will find the area of the three triangles we
	//make using the point, then add the three triangle areas together. If the area
	//of the three triangles added up is the same as the first triangles area, the point
	//is inside the triangle. If the area of the three triangles added up is greater than
	//the area of the first triangle, the point is outside the triangle.

	//Find area of first triangle
	float distX = triV1.x - triV2.x;
	float distY = triV1.y - triV2.y;
	float distZ = triV1.z - triV2.z;

	float edgeLength1 = sqrt(distX*distX + distY * distY + distZ * distZ);

	distX = triV1.x - triV3.x;
	distY = triV1.y - triV3.y;
	distZ = triV1.z - triV3.z;

	float edgeLength2 = sqrt(distX*distX + distY * distY + distZ * distZ);

	distX = triV2.x - triV3.x;
	distY = triV2.y - triV3.y;
	distZ = triV2.z - triV3.z;

	float edgeLength3 = sqrt(distX*distX + distY * distY + distZ * distZ);

	float s = (edgeLength1 + edgeLength2 + edgeLength3) / 2.0f;

	float mainTriArea = sqrt(s*(s - edgeLength1)*(s - edgeLength2)*(s - edgeLength3));

	//Find areas of the three triangles created with the point

	float smallTriArea[3] = { 0.0f, 0.0f, 0.0f };
	Vector3 triVert[4];
	triVert[0] = triV1;
	triVert[1] = triV2;
	triVert[2] = triV3;
	triVert[3] = triV1; //When i=2, i+1 will be triV1

	//Find 3 triangle areas using the plane intersecting point
	for (int i = 0; i < 3; i++)
	{
		distX = point.x - triVert[i].x;
		distY = point.y - triVert[i].y;
		distZ = point.z - triVert[i].z;

		edgeLength1 = sqrt(distX*distX + distY * distY + distZ * distZ);

		distX = point.x - triVert[i + 1].x;
		distY = point.y - triVert[i + 1].y;
		distZ = point.z - triVert[i + 1].z;

		edgeLength2 = sqrt(distX*distX + distY * distY + distZ * distZ);

		distX = triVert[i].x - triVert[i + 1].x;
		distY = triVert[i].y - triVert[i + 1].y;
		distZ = triVert[i].z - triVert[i + 1].z;

		edgeLength3 = sqrt(distX*distX + distY * distY + distZ * distZ);

		s = (edgeLength1 + edgeLength2 + edgeLength3) / 2.0f;

		smallTriArea[i] = sqrt(s*(s - edgeLength1)*(s - edgeLength2)*(s - edgeLength3));
	}

	float totalSmallTriArea = smallTriArea[0] + smallTriArea[1] + smallTriArea[2];

	//Compare the three smaller triangle areas with the main triangles area
	//Subtract a small value from totalSmallTriArea to make up for inacuracy
	if (mainTriArea >= (totalSmallTriArea - 0.001f))
	{
		return true;
	}

	return false;
}

bool Collider::CheckForTriangleIntersection(const Triangle& triangleA, const Triangle& triangleB) const
{
	return false;
}

bool Collider::CheckForPlaneIntersection(const Plane& pn1, const Plane& pn2) const
{
	Vector3   u = pn1.normal * pn2.normal;          // cross product
	float    ax = (u.x >= 0 ? u.x : -u.x);
	float    ay = (u.y >= 0 ? u.y : -u.y);
	float    az = (u.z >= 0 ? u.z : -u.z);

	// test if the two planes are parallel
	if ((ax + ay + az) < 0.001f) {        // Pn1 and Pn2 are near parallel
		// test if disjoint or coincide
		Vector3   v = pn2.origin - pn1.origin;
		if (glm::dot(pn1.normal, v) == 0)          // Pn2.V0 lies in Pn1
			return true;                    // Pn1 and Pn2 coincide
		else
			return false;                    // Pn1 and Pn2 are disjoint
	}

	// Pn1 and Pn2 intersect in a line
	// first determine max abs coordinate of cross product
	int      maxc;                       // max coordinate
	if (ax > ay) {
		if (ax > az)
			maxc = 1;
		else maxc = 3;
	}
	else {
		if (ay > az)
			maxc = 2;
		else maxc = 3;
	}

	// next, to get a point on the intersect line
	// zero the max coord, and solve for the other two
	Vector3    iP;                // intersect point
	float    d1, d2;            // the constants in the 2 plane equations
	d1 = -glm::dot(pn1.normal, pn1.origin);  // note: could be pre-stored  with plane
	d2 = -glm::dot(pn2.normal, pn2.origin);  // ditto

	switch (maxc) // select max coordinate
	{
		case 1:                     // intersect with x=0
			iP.x = 0;
			iP.y = (d2 * pn1.normal.z - d1 * pn2.normal.z) / u.x;
			iP.z = (d1 * pn2.normal.y - d2 * pn1.normal.y) / u.x;
			break;
		case 2:                     // intersect with y=0
			iP.x = (d1 * pn2.normal.z - d2 * pn1.normal.z) / u.y;
			iP.y = 0;
			iP.z = (d2 * pn1.normal.x - d1 * pn2.normal.x) / u.y;
			break;
		case 3:                     // intersect with z=0
			iP.x = (d2 * pn1.normal.y - d1 * pn2.normal.y) / u.z;
			iP.y = (d1 * pn1.normal.x - d2 * pn1.normal.x) / u.z;
			iP.z = 0;
			break;
	}

	//L->P0 = iP;
	//L->P1 = iP + u;

	ExtraRenderer::DrawLine(pn1.origin, pn1.normal);
	ExtraRenderer::DrawLine(pn2.origin, pn2.normal);

	return true;
}

bool Collider::CheckCollision(Collider& col)
{
	int triangleCountA = points.size() / 4;
	int triangleCountB = col.points.size() / 4;

	Vector3 offset = transform->position - col.transform->position;

	//std::cout << "Checking Count: " << triangleCountA << " & " << triangleCountB << "\n";
	//std::cout << "Pos: " << transform->position.y << " & " << col.transform->position.y << "\n";
	//std::cout << "Offset: " << offset.x << ", " << offset.y << ", " << offset.z << "\n";

	//Matrix4 transformMatrix = transform->GetTransformMatrix();
	//Matrix4 otherTransformMatrix = col.transform->GetTransformMatrix();

	for (int i = 0; i < planes.size(); ++i)
	{
		Plane planeA = planes[i];
		planeA.origin = planeA.origin + transform->position;
		for (int j = 0; j < col.planes.size(); ++j)
		{
			Plane planeB = col.planes[j];
			planeB.origin = planeB.origin + transform->position;

			if (CheckForPlaneIntersection(planeA, planeB))
			{
				//std::cout << "Collision detected\n";
				gameObject->OnCollisionEnter(col);
				return true;
			}
		}
	}

	return false;
}

void Collider::CreateConvex()
{
	if (mesh->vertices.size() > 0) {
		std::cout << mesh->vertices.size() << std::endl;

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
}

void Collider::SetMesh(Mesh& mesh)
{
	std::cout << "Mesh Renderer Vertices: " << mesh.vertices.size() << "\n";
	if (mesh.indices.size() > 0)
	{
		for (int i = 0; i < mesh.indices.size(); i += 3)
		{
			int indexA = mesh.indices[i];
			int indexB = mesh.indices[i + 1];
			int indexC = mesh.indices[i + 2];

			points.push_back(mesh.vertices[indexA]);
			points.push_back(mesh.vertices[indexB]);
			points.push_back(mesh.vertices[indexC]);

			Plane plane = Plane(mesh.vertices[indexA], mesh.vertices[indexB], mesh.vertices[indexC]);
			planes.push_back(plane);
		}
	}

	//points = mesh.vertices;
}

void Collider::DrawCollider()
{
	std::cout << "Drawing collider Name: " << transform->name << "\n";

	MeshRenderer meshRenderer = MeshRenderer();

	meshRenderer.transform->position = transform->position;
	meshRenderer.mesh.vertices = this->points;
	//meshRenderer.mesh.indices = this->pointIndices;

	meshRenderer.OnComponentAdded();
	meshRenderer.mesh.SetupMesh();

	meshRenderer.DrawMesh();
}
