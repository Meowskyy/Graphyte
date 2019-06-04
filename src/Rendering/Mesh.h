#pragma once

#include <string>
#include <vector>

#include "Math\Math.h"

#include <iostream>

class Mesh {
private:
	unsigned int VAO, VBO, EBO;
public:
	std::vector<Vector3> vertices = std::vector<Vector3>();
	std::vector<Vector3> normals = std::vector<Vector3>();
	std::vector<Vector2> uvs = std::vector<Vector2>();
	std::vector<unsigned int> indices;

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