#include "MeshRenderer.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Resources\ResourceManager.h"
#include "Rendering\Camera.h"
#include "Graphyte\Graphyte.h"

#include <iostream>

#include <stb_image.h>

#include "ExtraRenderer.h"

bool IsExtensionSupported(const char *name);

std::vector<Texture2D> loadMaterialTextures(const aiMaterial *mat, const aiTextureType type, const std::string typeName, const std::string directory);
unsigned int TextureFromFile(const char *path, const std::string &directory, const bool gamma = true);

void MeshRenderer::OnBehaviourAdded()
{
	RecalculateBoundingBox();
	// Set the transform position to center of mesh
	//glm::vec3 dimensions = transform->boundingBox.max - transform->boundingBox.min;
	//transform->position = transform->boundingBox.min + (dimensions / 2.0f);

	model = glm::scale(model, transform->scale);					// SCALE
	model = glm::translate(model, transform->GetWorldPosition());	// POSITION
	rot = glm::mat4_cast(transform->rotation);						// ROTATION
	model = model * rot;
}

void MeshRenderer::RecalculateBoundingBox()
{
	transform->boundingBox.min.y = 10000;


	for (int i = 0; i < mesh.vertices.size(); i++) 
	{
		if (mesh.vertices[i].x < transform->boundingBox.min.x) { transform->boundingBox.min.x = mesh.vertices[i].x; }
		if (mesh.vertices[i].x > transform->boundingBox.max.x) { transform->boundingBox.max.x = mesh.vertices[i].x; }
		if (mesh.vertices[i].y < transform->boundingBox.min.y) { transform->boundingBox.min.y = mesh.vertices[i].y; }
		if (mesh.vertices[i].y > transform->boundingBox.max.y) { transform->boundingBox.max.y = mesh.vertices[i].y; }
		if (mesh.vertices[i].z < transform->boundingBox.min.z) { transform->boundingBox.min.z = mesh.vertices[i].z; }
		if (mesh.vertices[i].z > transform->boundingBox.max.z) { transform->boundingBox.max.z = mesh.vertices[i].z; }
	}

	float centerX = (transform->boundingBox.min.x + transform->boundingBox.max.x) / 2;
	float centerY = (transform->boundingBox.min.y + transform->boundingBox.max.y) / 2;
	float centerZ = (transform->boundingBox.min.z + transform->boundingBox.max.z) / 2;
	Vector3 center = Vector3(centerX, centerY, centerZ);

	//transform->position = center;
}

void MeshRenderer::DrawUI()
{
	for (int i = 0; i < materials.size(); i++) {
		ImGui::Text("Shader: ");
		ImGui::SameLine();
		ImGui::Text(materials[i].shader.shaderName.c_str());

		ImGui::Text("Main Texture: ");
		ImGui::SameLine();
		ImGui::Text(materials[i].textures[0].name.c_str());
	}
}

MeshRenderer::MeshRenderer() {
	materials.push_back(Material());
}

MeshRenderer::MeshRenderer(aiMesh* mesh, const aiScene* scene, const std::string directory)
{
	// GET MATERIAL FROM RESOURCEMANAGER
	materials.push_back(Material());

	processMesh(mesh, scene, directory);
}

// TODO: Render only if visible
// TODO: Add a bounding box around the mesh
// TODO: Group objects with same shader
void MeshRenderer::Update()
{
	bool positionHasChanged = transform->positionHasChanged();
	bool rotationHasChanged = transform->rotationHasChanged();
	bool scaleHasChanged = transform->scaleHasChanged();
	
	if (positionHasChanged || rotationHasChanged || scaleHasChanged) 
	{
		// create transformations
		model = glm::mat4();
		model = glm::scale(model, transform->scale);					// SCALE
		model = glm::translate(model, transform->GetWorldPosition());	// POSITION

		if (rotationHasChanged) 
		{
			rot = glm::mat4_cast(transform->rotation);					// ROTATION
		}

		model = model * rot;
	}

	for (int i = 0; i < materials.size(); i++) {
		materials[i].shader.SetMatrix4("model", model, true);
	}

	materials[0].Use();
	mesh.Render();

	if (drawBoundingBox) {
		ExtraRenderer::DrawAABB(transform->boundingBox, transform->position);
	}

	// Unbind texture
	glActiveTexture(0);
}

// TODO: Group objects with same shader
void MeshRenderer::DrawLines()
{
	bool positionHasChanged = transform->positionHasChanged();
	bool rotationHasChanged = transform->rotationHasChanged();
	bool scaleHasChanged = transform->scaleHasChanged();

	if (positionHasChanged || rotationHasChanged || scaleHasChanged)
	{
		// create transformations
		model = glm::mat4();
		model = glm::scale(model, transform->scale);					// SCALE
		model = glm::translate(model, transform->GetWorldPosition());	// POSITION

		if (rotationHasChanged)
		{
			rot = glm::mat4_cast(transform->rotation);					// ROTATION
		}

		model = model * rot;
	}

	for (int i = 0; i < materials.size(); i++) {
		materials[i].shader.SetMatrix4("model", model, true);
	}

	materials[0].Use();
	mesh.RenderLines();

	// Unbind texture
	glActiveTexture(0);
}


// Reference Version
void MeshRenderer::processMesh(const aiMesh* mesh, const aiScene* scene, const std::string directory)
{
	// data to fill
	std::vector<Vector3> vertices;
	std::vector<Vector3> normals;
	std::vector<Vector2> uv;
	//std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture2D> textures;

	// Walk through each of the mesh's vertices
	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		//Vertex vertex;
		Vector3 vertex; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.

		// positions
		vertex.x = mesh->mVertices[i].x;
		vertex.y = mesh->mVertices[i].y;
		vertex.z = mesh->mVertices[i].z;
		vertices.push_back(vertex);

		//Vertex vertex;
		Vector3 normal; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
		normal.x = mesh->mNormals[i].x;
		normal.y = mesh->mNormals[i].y;
		normal.z = mesh->mNormals[i].z;
		normals.push_back(normal);

		// texture coordinates
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
	materials[0].textures = textures;

	// return a mesh object created from the extracted mesh data
	this->mesh = Mesh(vertices, uv, indices);
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