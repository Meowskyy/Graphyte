#pragma once

#include "Rendering\Material\Shader.h"

#include <string>
#include <vector>

struct Vertex {
	glm::vec3 Position;		// Vertex postion
	glm::vec3 Normal;		// Normal direction
	glm::vec2 TexCoords;	// Texture coordinates
	glm::vec3 Tangent;		// Tangent
	glm::vec3 Bitangent;	// Bitangent
};

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh {
public:
	/*  Mesh Data  */
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	/*  Functions  */
	// constructor
	Mesh() {};
	Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);

	// render the mesh
	void Render(Shader shader);

private:
	/*  Render data  */
	unsigned int VAO, VBO, EBO;

	/*  Functions    */
	// initializes all the buffer objects/arrays
	void setupMesh();
};