#include "Mesh.h"

#include <iostream>
#include <exception>

#include <glad/glad.h> 

#include <glm\gtx\normal.hpp>

using namespace Graphyte;

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

Vector3 Mesh::triangleNormalFromVertex(int face_id, int vertex_id) {
	//This assumes that A->B->C is a counter-clockwise ordering
	Vector3 A = faces[face_id].vertex[vertex_id];
	Vector3 B = faces[face_id].vertex[(vertex_id + 1) % 3];
	Vector3 C = faces[face_id].vertex[(vertex_id + 2) % 3];


	Vector3 N = glm::cross(B - A, C - A);
	float sin_alpha = glm::length(N) / (glm::length(B - A) * glm::length(C - A));
	return glm::normalize(N) * glm::asin(sin_alpha);
}

int Mesh::index_of_v_in_triangle(int i, Vector3 vertex) {
	for (int i = 0; i < indices.size(); i++) {
		if (vertices[indices[i]] == vertex) 
		{
			return i % 3;
		}
	}
}

void Mesh::RecalculateNormals()
{
	normals.clear();

	if (indices.size() > 0) {
		for (int i = 0; i < vertices.size(); i++)
		{
			normals.push_back(Vector3(1, 1, 1));
		}

		/*
		for (int i = 0; i < indices.size() / 3; ++i)
		{
			Triangle triangle;

			triangle.vertex[0] = vertices[indices[i * 3 + 0]];
			triangle.vertex[1] = vertices[indices[i * 3 + 1]];
			triangle.vertex[2] = vertices[indices[i * 3 + 2]];
			triangle.vertex[3] = vertices[indices[i * 3 + 3]];

			triangle.vertexID[0] = indices[i * 3 + 0];
			triangle.vertexID[1] = indices[i * 3 + 1];
			triangle.vertexID[2] = indices[i * 3 + 2];
			triangle.vertexID[3] = indices[i * 3 + 3];

			triangles.push_back(triangle);
		}
		*/

		for (int i = 0; i < indices.size() / 3; ++i)
		{
			Triangle triangle;

			triangle.vertex[0] = vertices[indices[i * 3 + 0]];
			triangle.vertex[1] = vertices[indices[i * 3 + 1]];
			triangle.vertex[2] = vertices[indices[i * 3 + 2]];
			triangle.vertex[3] = vertices[indices[i * 3 + 0]];

			triangle.vertexID[0] = indices[i * 3 + 0];
			triangle.vertexID[1] = indices[i * 3 + 1];
			triangle.vertexID[2] = indices[i * 3 + 2];
			triangle.vertexID[3] = indices[i * 3 + 0];

			triangles.push_back(triangle);
		}

		for (int i = 0; i < indices.size() / 6; ++i)
		{
			Face face;

			face.vertex[0] = vertices[indices[i * 6 + 0]];
			face.vertex[1] = vertices[indices[i * 6 + 1]];
			face.vertex[2] = vertices[indices[i * 6 + 2]];
			face.vertex[3] = vertices[indices[i * 6 + 3]];

			face.vertexID[0] = indices[i * 6 + 0];
			face.vertexID[1] = indices[i * 6 + 1];
			face.vertexID[2] = indices[i * 6 + 2];
			face.vertexID[3] = indices[i * 6 + 3];

			faces.push_back(face);
		}

		int faceCount = indices.size() / 6;
		int triangleCount = indices.size() / 3;

		std::cout << "FaceCount: " << faceCount << "\n";
		std::cout << "TriangleCount: " << triangleCount << "\n";

		for (int i = 0; i < triangleCount; i++)
		{
			// FOR EACH VERTEX IN TRIANGLE
			for (int j = 0; j < 3; j++) 
			{
				Vector3 vertexA = (j == 0) ? triangles[i].vertex[3] : triangles[i].vertex[j - 1];
				Vector3 vertexB = triangles[i].vertex[j];
				Vector3 vertexC = (j == 2) ? triangles[i].vertex[0] : triangles[i].vertex[j + 1];

				//Vector3 normal = glm::normalize(glm::cross(vertexC - vertexA, vertexB - vertexA));

				Vector3 normal = glm::normalize((vertexA + vertexB + vertexC) / 3.0f);

				Vector3 V1 = (vertexC - vertexB);
				Vector3 V2 = (vertexA - vertexB);
				Vector3 surfaceNormal;
				surfaceNormal.x = (V1.y * V2.z) - (V1.z - V2.y);
				surfaceNormal.y = -((V2.z * V1.x) - (V2.x * V1.z));
				surfaceNormal.z = (V1.x - V2.y) - (V1.y - V2.x);

				// Dont forget to normalize if needed
				surfaceNormal = glm::normalize(surfaceNormal);

				normals[triangles[i].vertexID[j]] = normal;
			}
		}


		for (int i = 0; i < vertices.size(); i += 4)
		{
			Vector3 normal = glm::normalize(glm::cross(vertices[i + 1] - vertices[i], vertices[i + 2] - vertices[i]) * Vector3(0, 1, 0));

			normals[i] = normal;
			normals[i + 1] = normal;
			normals[i + 2] = normal;
			normals[i + 3] = normal;
		}
	}
	else {
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

	GLfloat lineWidthRange[2] = { 0.0f, 0.0f };

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
