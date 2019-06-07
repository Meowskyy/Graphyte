#include "Mesh.h"

#include <iostream>
#include <exception>

#include <glad/glad.h> 

Mesh::Mesh(const std::vector<Vector3> vertices, const std::vector<Vector3> normals, const std::vector<Vector2> uvs, const std::vector<unsigned int> indices)
{
	this->vertices = vertices;
	this->indices = indices;
	this->uvs = uvs;
	this->normals = normals;

	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	SetupMesh();
}
Mesh::Mesh(const std::vector<Vector3> vertices, const std::vector<Vector2> uvs, const std::vector<unsigned int> indices)
{
	this->vertices = vertices;
	this->indices = indices;
	this->uvs = uvs;
	//this->normals = std::vector<Vector3>(0);

	// now that we have all the required data, set the vertex buffers and its attribute pointers.
	SetupMesh();
}

Mesh::~Mesh() {
	//glDeleteVertexArrays(1, &VAO);

	//glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
}

void Mesh::Bind() const
{
	// Bind VAO
	glBindVertexArray(VAO);
}

void Mesh::RecalculateNormals()
{
	normals.clear();

	for (int i = 0; i < vertices.size(); i++)
	{
		normals.push_back(Vector3(1, 1, 1));
	}

	for (int i = 0; i < vertices.size(); i += 4)
	{
		Vector3 normal = glm::normalize(glm::cross(vertices[i + 1] - vertices[i], vertices[i + 2] - vertices[i]));

		normals[i] = normal;
		normals[i + 1] = normal;
		normals[i + 2] = normal;
		normals[i + 3] = normal;
	}

}

void Mesh::RenderLines()
{
#pragma region CreateBuffers
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// VERTEX BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vector3), &vertices[0], GL_STATIC_DRAW);

	// INDEX BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*)0);
#pragma endregion

	glEnable(GL_LINE_SMOOTH);

	glBindVertexArray(VAO);

	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, 0);
	glDrawElements(GL_LINE_LOOP, 4, GL_UNSIGNED_INT, (GLvoid*)(4 * sizeof(unsigned int)));
	glDrawElements(GL_LINES, 8, GL_UNSIGNED_INT, (GLvoid*)(8 * sizeof(unsigned int)));

	// Unbinding VAO
	glBindVertexArray(0);

	// Delete all buffers at the end
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDisable(GL_LINE_SMOOTH);
}

void Mesh::RenderLine(const Vector3 color) 
{
#pragma region CreateBuffers
	// create buffers/arrays
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	// VERTEX BUFFER
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vector3), &vertices[0], GL_STATIC_DRAW);

	// INDEX BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

	// vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*)0);
#pragma endregion

	glEnable(GL_LINE_SMOOTH);
	glColor3f(color.x, color.y, color.z);

	glBindVertexArray(VAO);

	glDrawElements(GL_LINES, 2, GL_UNSIGNED_INT, 0);

	// Unbinding VAO
	glBindVertexArray(0);

	// Delete all buffers at the end
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glDisable(GL_LINE_SMOOTH);
}

void Mesh::SetupMesh()
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
		//std::cerr << "index count 0" << std::endl;
	}

	if (vertexArraySize > 0) {
		glBufferData(GL_ARRAY_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);

		// TODO: Checking if everything works properly and adding normals
		// Vertex positions
			glBufferSubData(GL_ARRAY_BUFFER, 0, vertexArraySize, &vertices[0]);
		// Normal positions
		if (normalArraySize > 0) // TODO: Do this somehow else
			glBufferSubData(GL_ARRAY_BUFFER, vertexArraySize, normalArraySize, &normals[0]);

		// UV positions
		if (uvArraySize > 0) // TODO: Do this somehow else
			glBufferSubData(GL_ARRAY_BUFFER, vertexArraySize + normalArraySize, uvArraySize, &uvs[0]);
		
		// VBO END
		if (indices.size() > 0) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		}

		// set the vertex attribute pointers
		// vertex Positions
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*)0);
		// vertex normals
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vector3), (void*)(vertexArraySize));
		// vertex texture coords
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vector2), (void*)(vertexArraySize + normalArraySize));
	}

	glBindVertexArray(0);
}
