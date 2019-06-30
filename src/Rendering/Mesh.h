#pragma once

#include <string>
#include <vector>

#include "Math\Math.h"

#include <iostream>

namespace Graphyte {
	class Mesh {
		class Triangle
		{
		public:
			Vector3 vertex[4];
			int vertexID[4];
		};

		class Face
		{
		public:
			Vector3 vertex[4];
			int vertexID[4];
		};

	private:
		unsigned int VAO, VBO, EBO;

		Vector3 triangleNormalFromVertex(int face_id, int vertex_id);
		int index_of_v_in_triangle(int i, Vector3 vertex);

		std::vector<Triangle> triangles = std::vector<Triangle>();
		std::vector<Face> faces = std::vector<Face>();

	public:
		std::vector<Vector3> vertices = std::vector<Vector3>();
		std::vector<Vector3> normals = std::vector<Vector3>();
		std::vector<Vector2> uvs = std::vector<Vector2>();
		std::vector<unsigned int> indices;

		/*
		// assume the object holds reusable storage, such as a heap-allocated buffer mArray
		Mesh& operator=(const Mesh& other) // copy assignment
		{
			return *this;
		}
		*/

		// constructor
		Mesh() = default;
		~Mesh();
		Mesh(const std::vector<Vector3> vertices, const std::vector<Vector3> normals, const std::vector<Vector2> uvs, const std::vector<unsigned int> indices);
		Mesh(const std::vector<Vector3> vertices, const std::vector<Vector2> uvs, const std::vector<unsigned int> indices);

		// Bind the mesh
		void Bind() const;

		void RecalculateNormals();

		// Extra options
		void RenderLines();
		void RenderLine(const Vector3 color);

		// initializes all the buffer objects/arrays
		void SetupMesh();
	};
}