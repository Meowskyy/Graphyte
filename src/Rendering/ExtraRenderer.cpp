#include "ExtraRenderer.h"

#include "Resources\ResourceManager.h"
#include "Objects\Scene.h"

#include "GLFW\glfw3.h"
#include "glm\vec4.hpp"

#include <vector>

#include "Rendering\MeshRenderer.h"

#include "glad\glad.h"

using namespace Graphyte;

float planeVertices[] = {
	// positions          // texture Coords 
	 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
	-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

	 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
	-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
	 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
};

void ExtraRenderer::DrawAABB(const AxisAlignedBoundingBox& box) 
{
	MeshRenderer meshRenderer = MeshRenderer();

	meshRenderer.mesh.vertices = std::vector<Vector3>
	{
		Vector3(-box.halfSize.x, -box.halfSize.y,  box.halfSize.z),	// OK
		Vector3(box.halfSize.x, -box.halfSize.y,  box.halfSize.z),	// OK
		box.halfSize,									// OK
		Vector3(-box.halfSize.x, box.halfSize.y,  box.halfSize.z),	// OK

		-box.halfSize,									// OK
		Vector3(box.halfSize.x, -box.halfSize.y, -box.halfSize.z),	// OK
		Vector3(box.halfSize.x, box.halfSize.y, -box.halfSize.z),	// OK
		Vector3(-box.halfSize.x, box.halfSize.y, -box.halfSize.z)	// OK
	};

	meshRenderer.mesh.indices = std::vector<unsigned int>
	{
		0, 1, 2, 3,
		4, 5, 6, 7,
		0, 4, 1, 5,
		2, 6, 3, 7
	};

	meshRenderer.transform->position = box.center;

	meshRenderer.OnComponentAdded();

	ResourceManager::GetShader("Unlit").SetVector3f("Color", Vector3(1, 1, 1), true);

	ResourceManager::GetShader("Unlit").SetMatrix4("view", Camera::mainCamera->GetViewMatrix());

	meshRenderer.DrawLines();
}

void ExtraRenderer::DrawBounds(const Bounds& box) {
	MeshRenderer meshRenderer = MeshRenderer();

	meshRenderer.mesh.vertices = std::vector<Vector3>
	{
		Vector3(box.min.x, box.min.y,  box.max.z),	// OK
		Vector3(box.max.x, box.min.y,  box.max.z),	// OK
		box.max,									// OK
		Vector3(box.min.x, box.max.y,  box.max.z),	// OK

		box.min,											// OK
		Vector3(box.max.x, box.min.y, box.min.z),	// OK
		Vector3(box.max.x, box.max.y, box.min.z),	// OK
		Vector3(box.min.x, box.max.y, box.min.z)	// OK
	};

	meshRenderer.mesh.indices = std::vector<unsigned int>
	{
		0, 1, 2, 3,
		4, 5, 6, 7,
		0, 4, 1, 5,
		2, 6, 3, 7
	};

	meshRenderer.OnComponentAdded();

	ResourceManager::GetShader("Unlit").SetVector3f("Color", Vector3(1, 1, 1), true);
	ResourceManager::GetShader("Unlit").SetMatrix4("view", Camera::mainCamera->GetViewMatrix());

	meshRenderer.DrawLines();
}

void ExtraRenderer::DrawLine(const Vector3& start, const Vector3& end, const Vector3& color)
{
	MeshRenderer meshRenderer = MeshRenderer();

	meshRenderer.mesh.vertices = std::vector<Vector3> 
	{
		start,
		start + end
	};

	meshRenderer.mesh.indices = std::vector<unsigned int>
	{
		0, 1
	};

	meshRenderer.OnComponentAdded();

	ResourceManager::GetShader("Unlit").SetVector3f("Color", color, true);
	meshRenderer.DrawLine(color);
}

void ExtraRenderer::DrawSelectionArrows(const Transform& transf, const float distance)
{
	glDisable(GL_DEPTH_TEST);
	DrawLine(transf.position, transf.GetForwardVector() * distance, Vector3(0, 1, 0));
	DrawLine(transf.position, transf.GetRightVector() * distance, Vector3(1, 0, 0));
	DrawLine(transf.position, transf.GetUpVector() * distance, Vector3(0, 0, 1));
}

void ExtraRenderer::DrawNormals(const MeshRenderer& meshRenderer)
{
	// TODO: Fixing normal drawing
	for (int i = 0; i < meshRenderer.mesh.vertices.size(); i++)
	{
		DrawLine(meshRenderer.mesh.vertices[i] * meshRenderer.transform->scale + meshRenderer.transform->position, meshRenderer.mesh.normals[i], Vector3(1, 1, 1));
	}

}

void ExtraRenderer::DrawTextureOnScreen(Texture2D& texture) 
{
	glDisable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
	MeshRenderer meshRenderer = MeshRenderer();

	meshRenderer.mesh.vertices = std::vector<Vector3>
	{
		Vector3(-1.0f,  1.0f, 0.0f),
		Vector3(-1.0f, -1.0f, 0.0f),
		Vector3(1.0f, -1.0f, -0.0f),

		Vector3(-1.0f,  1.0f, 0.0f),
		Vector3(1.0f, -1.0f, -0.0f),
		Vector3(1.0f,  1.0f, -0.0f)
	};

	meshRenderer.mesh.uvs = std::vector<Vector2>
	{
		Vector2(0.0f, 1.0f),
		Vector2(0.0f, 0.0f),
		Vector2(1.0f, 0.0f),

		Vector2(0.0f, 1.0f),
		Vector2(1.0f, 0.0f),
		Vector2(1.0f, 1.0f)
	};

	meshRenderer.mesh.indices = std::vector<unsigned int>
	{
		0, 1, 2,
		3, 4, 5
	};

	meshRenderer.mesh.SetupMesh();

	meshRenderer.OnComponentAdded();

	texture.Bind();
	ResourceManager::GetShader("ScreenShader").Use();

	meshRenderer.DrawMesh();
}

void ExtraRenderer::DrawTextureOnScreen(GLuint texture)
{
	glDisable(GL_DEPTH_TEST); // enable depth testing (is disabled for rendering screen-space quad)
	MeshRenderer meshRenderer = MeshRenderer();

	meshRenderer.mesh.vertices = std::vector<Vector3>
	{
		Vector3(-1.0f,  1.0f, 0.0f),
		Vector3(-1.0f, -1.0f, 0.0f),
		Vector3(1.0f, -1.0f, -0.0f),

		Vector3(-1.0f,  1.0f, 0.0f),
		Vector3(1.0f, -1.0f, -0.0f),
		Vector3(1.0f,  1.0f, -0.0f)
	};

	meshRenderer.mesh.uvs = std::vector<Vector2>
	{
		Vector2(0.0f, 1.0f),
		Vector2(0.0f, 0.0f),
		Vector2(1.0f, 0.0f),

		Vector2(0.0f, 1.0f),
		Vector2(1.0f, 0.0f),
		Vector2(1.0f, 1.0f)
	};

	meshRenderer.mesh.indices = std::vector<unsigned int>
	{
		0, 1, 2,
		3, 4, 5
	};

	meshRenderer.mesh.SetupMesh();

	meshRenderer.OnComponentAdded();

	glBindTexture(GL_TEXTURE_2D, texture);
	ResourceManager::GetShader("ScreenShader").Use();

	meshRenderer.DrawMesh();
}