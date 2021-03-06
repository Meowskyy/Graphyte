#pragma once

#include <string>
#include <vector>

#include "Math\Vector3.h"
#include "Math\Vector2.h"

class Mesh {
public:
	/*  Mesh Data  */
	std::vector<Vector3> vertices = std::vector<Vector3>();
	std::vector<Vector3> normals = std::vector<Vector3>();
	std::vector<Vector2> uvs = std::vector<Vector2>();
	std::vector<unsigned int> indices;

	/*  Functions  */
	// constructor
	Mesh() {};
	Mesh(const std::vector<Vector3> vertices, const std::vector<Vector2> uvs, const std::vector<unsigned int> indices);

	// render the mesh
	void Render();

	// initializes all the buffer objects/arrays
	void setupMesh();

private:
	/*  Render data  */
	unsigned int VAO, VBO, EBO;
};