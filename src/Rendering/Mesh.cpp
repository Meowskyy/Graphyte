#include "Mesh.h"

#include <iostream>
#include <exception>

#include <glad/glad.h> 

Mesh::Mesh(const std::vector<Vector3> vertices, const std::vector<Vector2> uvs, const std::vector<unsigned int> indices)
{
	this->vertices = vertices;
	this->indices = indices;
	this->uvs = uvs;
	//this->normals = std::vector<Vector3>(0);

	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	setupMesh();
}

Mesh::~Mesh() {
	glDeleteVertexArrays(1, &VAO);

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

void Mesh::Render()
{
	// draw mesh
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	// Unbinding VAO
	glBindVertexArray(0);
}

void Mesh::RenderLines() {
	glBindVertexArray(VAO);

	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (GLvoid*)(4 * sizeof(unsigned int)));
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (GLvoid*)(8 * sizeof(unsigned int)));

	//glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (GLvoid*)(3 * sizeof(int)));
	//glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);

	// Unbinding VAO
	glBindVertexArray(0);
}

void Mesh::setupMesh()
{
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// TODO: Will crash if any array is not setup
	// load data into vertex buffers
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	int vertexArraySize = vertices.empty() ? 0 : vertices.size() * sizeof(Vector3);
	int normalArraySize = normals.empty() ? 0 : normals.size() * sizeof(Vector3);
	int uvArraySize = uvs.empty() ? 0 : uvs.size() * sizeof(Vector2);

	int bufferSize = vertexArraySize + normalArraySize + uvArraySize;

	if (indices.size() == 0) {
		std::cerr << "index count 0" << std::endl;
	}

	if (vertexArraySize > 0) {
		glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

		// TODO: Checking if everything works properly and adding normals
		// Vertex positions
		if (vertexArraySize > 0) // TODO: Do this somehow else
			glBufferSubData(GL_ARRAY_BUFFER, 0, vertexArraySize, &vertices[0]);
		// Normal positions
		if (normalArraySize > 0) // TODO: Do this somehow else
			glBufferSubData(GL_ARRAY_BUFFER, vertexArraySize, normalArraySize, &normals[0]);

		// UV positions
		if (uvArraySize > 0) // TODO: Do this somehow else
			glBufferSubData(GL_ARRAY_BUFFER, vertexArraySize + normalArraySize, uvArraySize, &uvs[0]);
		// VBO END

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*)0);
		// vertex normals
		//glEnableVertexAttribArray(1);
		//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)(vertexArraySize));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), (void*)(vertexArraySize + normalArraySize));
	}

	glBindVertexArray(0);
}
