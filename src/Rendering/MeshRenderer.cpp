#include "MeshRenderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Resources\ResourceManager.h"
#include "Rendering\Camera.h"
#include "Graphyte\Graphyte.h"

#include <iostream>

#include <stb_image.h>

#include "ExtraRenderer.h"

#include "imgui.h"

#include "Camera.h"

using namespace Graphyte;

bool IsExtensionSupported(const char *name);

std::vector<Texture2D> loadMaterialTextures(const aiMaterial *mat, const aiTextureType type, const std::string typeName, const std::string directory);
unsigned int TextureFromFile(const char *path, const std::string &directory, const bool gamma = true);

void MeshRenderer::OnComponentAdded()
{
	RecalculateBoundingBox();
}

void MeshRenderer::RecalculateBoundingBox()
{
	Vector3 min;
	Vector3 max;

	if (mesh.vertices.size() > 0) {
		min.x = max.x = mesh.vertices[0].x;
		min.y = max.y = mesh.vertices[0].y;
		min.z = max.z = mesh.vertices[0].z;
	}

	for (auto& vertex : mesh.vertices) 
	{
		if (vertex.x < min.x) { min.x = vertex.x; }
		if (vertex.x > max.x) { max.x = vertex.x; }
		if (vertex.y < min.y) { min.y = vertex.y; }
		if (vertex.y > max.y) { max.y = vertex.y; }
		if (vertex.z < min.z) { min.z = vertex.z; }
		if (vertex.z > max.z) { max.z = vertex.z; }
	}

	//std::cout << "Min: " << min.x << std::endl;
	//std::cout << "Max: " << max.x << std::endl;

	transform->boundingBox = BoundingBox(min, max);
	//transform->position = center;
}

void MeshRenderer::DrawUI()
{
	ImGui::Checkbox("Draw mesh", &drawMesh);
	ImGui::Checkbox("Draw bounding box", &drawBoundingBox);
	ImGui::Checkbox("Is visible", &isVisible);

	for (int i = 0; i < materials.size(); i++) 
	{
		ImGui::Text("Material: ");
		ImGui::SameLine();
		ImGui::Text(materials[i].name.c_str());

		ImGui::Text("Shader: ");
		ImGui::SameLine();
		ImGui::Text(materials[i].shader.shaderName.c_str());

		ImGui::ColorEdit3("Object Color", (float*)&objectColor);
		ImGui::Text("Main Texture: ");
		ImGui::SameLine();
		for (auto& texture : materials[i].textures) 
		{
			ImGui::Text(texture.name.c_str());
		}

		ImGui::DragFloat("Specular Intensity", &specularIntensity, 0.05f, 0.0, 1.0f);
		ImGui::DragFloat("Specular Power", &specularPower);

		ImGui::DragFloat("Roughness", &roughness, 0.05f, 0.0, 1.0f);
		ImGui::DragFloat("Metallic", &metallic, 0.05f, 0.0, 1.0f);
		ImGui::DragFloat("AO", &ao, 0.05f, 0.0, 1.0f);
	}
}

MeshRenderer::MeshRenderer() {
	mesh = Mesh();
	materials.push_back(Material());
}

MeshRenderer::MeshRenderer(const aiMesh* mesh, const aiScene* scene, const std::string directory)
{
	// GET MATERIAL FROM RESOURCEMANAGER
	materials.push_back(Material());

	processMesh(mesh, scene, directory);
}

void MeshRenderer::Render(Camera& camera)
{
	// TODO: Update only if camera moves/rotates or this moves/rotates/scales
	// If mesh is visible in mainCamera
	this->isVisible = camera.frustrum.TestIntersection(*transform);

	/*
	if (
		Camera::mainCamera->transform->positionHasChanged() || Camera::mainCamera->transform->rotationHasChanged() ||
		this->transform->positionHasChanged() || this->transform->rotationHasChanged() || this->transform->scaleHasChanged()
		) 
	{
		this->isVisible = Camera::mainCamera->frustrum.TestIntersection(*transform);
	}
	*/

	//std::cout << "Rendering " << transform->name << " with shader: " << Shader::currentShader << std::endl;
	if (isVisible && drawMesh && mesh.vertices.size() > 0)
	{
		// TODO: Bugged
		//materials[0].Use();

		Matrix4 RotationMatrix = glm::mat4_cast(transform->rotation);
		Matrix4 TranslationMatrix = glm::translate(Matrix4(), transform->position);
		Matrix4 ScaleMatrix = glm::scale(Matrix4(), transform->scale);
		Matrix4 ModelMatrix = TranslationMatrix * RotationMatrix * ScaleMatrix;

		for (int i = 0; i < materials.size(); i++) {
			materials[i].shader.SetMatrix4("model", ModelMatrix);
			materials[i].shader.SetVector3f("objectColor", objectColor);
			materials[i].shader.SetFloat("specularIntensity", specularIntensity);
			materials[i].shader.SetFloat("specularPower", specularPower);

			materials[i].shader.SetVector3f("albedo", objectColor);
			materials[i].shader.SetFloat("roughness", roughness);
			materials[i].shader.SetFloat("metallic", metallic);
			materials[i].shader.SetFloat("ao", ao);
			//materials[i].shader.SetBool("blinn", false);
		}

		// Bind VAO
		mesh.Bind();
		// draw mesh
		if (mesh.indices.size() > 0) {
			glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
		}
		else {
			glDrawArrays(GL_TRIANGLES, 0, mesh.vertices.size());
		}

		// Unbinding VAO
		glBindVertexArray(0);

		// TODO: Remove from release
		RenderExtras();
	}
}

void MeshRenderer::RenderExtras() 
{
	if (drawBoundingBox) {
		ExtraRenderer::DrawAABB(transform->boundingBox, transform->position);
	}
}

void MeshRenderer::DrawLines()
{
	// create transformations
	glm::mat4 model = glm::mat4();
	model = glm::scale(model, transform->scale);					// SCALE
	model = glm::translate(model, transform->GetWorldPosition());	// POSITION
	glm::mat4 rot = glm::mat4_cast(transform->rotation);			// ROTATION

	model = model * rot;

	ResourceManager::GetShader("Unlit").SetMatrix4("model", model);

	mesh.RenderLines();
}

void MeshRenderer::DrawLine(const Vector3 color)
{
	// create transformations
	glm::mat4 model = glm::mat4();
	model = glm::scale(model, transform->scale);					// SCALE
	model = glm::translate(model, transform->GetWorldPosition());	// POSITION
	glm::mat4 rot = glm::mat4_cast(transform->rotation);			// ROTATION

	model = model * rot;

	ResourceManager::GetShader("Unlit").SetMatrix4("model", model);

	mesh.RenderLine(color);
}

void MeshRenderer::processMesh(const aiMesh* mesh, const aiScene* scene, const std::string directory)
{
	// data to fill
	std::vector<Vector3> vertices;
	std::vector<Vector3> normals;
	std::vector<Vector2> uv;
	std::vector<unsigned int> indices;
	std::vector<Texture2D> textures;

	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		// Position
		Vector3 vertex; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		vertex.x = mesh->mVertices[i].x;
		vertex.y = mesh->mVertices[i].y;
		vertex.z = mesh->mVertices[i].z;
		vertices.push_back(vertex);

		// Normal
		Vector3 normal; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;
		normals.push_back(normal);

		// UV Coordinates
		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			Vector2 vec;
			// a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
			// use models where a vertex can have multiple texture coordinates so we always take the first set (0).
			vec.x = mesh->mTextureCoords[0][i].x;
			vec.y = mesh->mTextureCoords[0][i].y;
			uv.push_back(vec);
		}
		else {
			uv.push_back(Vector2(0.0f, 0.0f));
		}

		/*
		// tangent
		vector.x = mesh->mTangents[i].x;
		vector.y = mesh->mTangents[i].y;
		vector.z = mesh->mTangents[i].z;
		vertex.Tangent = vector;
		// bitangent
		vector.x = mesh->mBitangents[i].x;
		vector.y = mesh->mBitangents[i].y;
		vector.z = mesh->mBitangents[i].z;
		vertex.Bitangent = vector;
		*/
	}
	// now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		// retrieve all indices of the face and store them in the indices vector
		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}
	// process materials
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	// we assume a convention for sampler names in the shaders. Each diffuse texture should be named
	// as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
	// Same applies to other texture as the following list summarizes:
	// diffuse: texture_diffuseN
	// specular: texture_specularN
	// normal: texture_normalN

	std::string name = mesh->mName.C_Str();
	name += "_material";

	// 1. diffuse maps
	std::vector<Texture2D> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", directory);
	textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
	// 2. specular maps
	std::vector<Texture2D> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", directory);
	textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
	// 3. normal maps
	std::vector<Texture2D> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", directory);
	textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
	// 4. height maps
	std::vector<Texture2D> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", directory);
	textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

	// TODO: Being able to Assign multiple different materials
	//materials[0].textures = textures;

	Material mat = ResourceManager::LoadMaterial(mesh->mName.C_Str());
	mat.textures = textures;
	SetMaterial(mat);

	this->mesh = Mesh(vertices, normals, uv, indices);
	//this->mesh.RecalculateNormals();

}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
std::vector<Texture2D> loadMaterialTextures(const aiMaterial *mat, const aiTextureType type, const std::string typeName, const std::string directory)
{
	std::vector<Texture2D> textures;
	std::vector<Texture2D> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);
		// check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
		bool skip = false;
		for (unsigned int j = 0; j < textures_loaded.size(); j++)
		{
			if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
			{
				textures.push_back(textures_loaded[j]);
				skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
				break;
			}
		}
		if (!skip)
		{   // if texture hasn't been loaded already, load it
			Texture2D texture;
			texture.ID = TextureFromFile(str.C_Str(), directory);
			texture.type = typeName;
			texture.name = typeName;
			texture.path = str.C_Str();
			textures.push_back(texture);
			textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
		}
	}
	return textures;
}

unsigned int TextureFromFile(const char* path, const std::string &directory, const bool gamma)
{
	std::string filename = std::string(path);
	filename = directory + '/' + filename;

	std::cout << filename << std::endl;

	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		if (IsExtensionSupported("GL_EXT_texture_filter_anisotropic")) {
			//std::cout << "Anisotropic Filtering: SUPPORTED" << std::endl;

			//float aniso = 0.0f;
			//glGetFloatv(GL_MAX_TEXTURE_A, &aniso);
			//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
		}
		else {
			std::cout << "Anisotropic Filtering: NOT SUPPORTED" << std::endl;
		}

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

bool IsExtensionSupported(const char* name)
{
	GLint n = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &n);

	PFNGLGETSTRINGIPROC glGetStringi = 0;
	glGetStringi = (PFNGLGETSTRINGIPROC)glfwGetProcAddress("glGetStringi");

	/*
	for (GLint i = 0; i<n; i++)
	{
		const char* extension =
			(const char*)glGetStringi(GL_EXTENSIONS, i);
		printf("Ext %d: %s\n", i, extension);
	}
	*/

	for (GLint i = 0; i<n; i++)
	{
		const char* extension =
			(const char*)glGetStringi(GL_EXTENSIONS, i);
		if (!strcmp(name, extension))
		{
			return true;
		}
	}
	return false;
}