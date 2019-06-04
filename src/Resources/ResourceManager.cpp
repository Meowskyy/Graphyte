#include "ResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// TODO: Temporary
#include "glm\gtc\matrix_transform.hpp"

using namespace Graphyte;

// Instantiate static variables
std::map<std::string, Texture2D>    ResourceManager::Textures;
std::map<std::string, Shader>       ResourceManager::Shaders;
std::map<std::string, Material>     ResourceManager::Materials;

void ResourceManager::UpdateProjection(Matrix4 projection)
{
	// Set projection uniform in all shaders
	for (auto const&[key, val] : Shaders)
	{
		Shaders[key].SetMatrix4("projection", projection, true);
	}
}

void ResourceManager::UpdateView(Matrix4 view)
{
	// Set projection uniform in all shaders
	for (auto const&[key, val] : Shaders)
	{
		Shaders[key].SetMatrix4("view", view, true);
	}
}

Shader ResourceManager::LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile, const std::string name)
{
	Shaders[name] = loadShaderFromFile(vShaderFile, fShaderFile, gShaderFile);
	Shaders[name].shaderName = name;

	return Shaders[name];
}

Shader ResourceManager::GetShader(const std::string name)
{
	return Shaders[name];
}

Texture2D ResourceManager::LoadTexture(const GLchar *file, const bool gamma, const bool alpha, const std::string name)
{
	Textures[name] = loadTextureFromFile(file, alpha, gamma);
	Textures[name].name = name;

	return Textures[name];
}

Texture2D ResourceManager::GetTexture(const std::string name)
{
	return Textures[name];
}

Material ResourceManager::LoadMaterial(const std::string name)
{
	Materials[name] = Material();
	Materials[name].name = name;
	return Materials[name];
}

Material &ResourceManager::GetMaterial(const std::string name)
{
	return Materials[name];
}

void ResourceManager::Clear()
{
	// (Properly) delete all shaders	
	for (auto iter : Shaders)
		glDeleteProgram(iter.second.ID);
	// (Properly) delete all textures
	for (auto iter : Textures)
		glDeleteTextures(1, &iter.second.ID);
}

Shader ResourceManager::loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile)
{
	// 1. Retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::string geometryCode;
	try
	{
		// Open files
		std::ifstream vertexShaderFile(vShaderFile);
		std::ifstream fragmentShaderFile(fShaderFile);
		std::stringstream vShaderStream, fShaderStream;
		// Read file's buffer contents into streams
		vShaderStream << vertexShaderFile.rdbuf();
		fShaderStream << fragmentShaderFile.rdbuf();
		// close file handlers
		vertexShaderFile.close();
		fragmentShaderFile.close();
		// Convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
		// If geometry shader path is present, also load a geometry shader
		if (gShaderFile != nullptr)
		{
			std::ifstream geometryShaderFile(gShaderFile);
			std::stringstream gShaderStream;
			gShaderStream << geometryShaderFile.rdbuf();
			geometryShaderFile.close();
			geometryCode = gShaderStream.str();
		}
	}
	catch (std::exception e)
	{
		std::cout << "ERROR IN FILE: " << fShaderFile << std::endl;
		std::cout << "ERROR::SHADER: Failed to read shader files" << std::endl;
	}
	const GLchar *vShaderCode = vertexCode.c_str();
	const GLchar *fShaderCode = fragmentCode.c_str();
	const GLchar *gShaderCode = geometryCode.c_str();

	std::cout << "COMPILING FILE: " << fShaderFile << std::endl;
	// 2. Now create shader object from source code
	Shader shader;
	shader.Compile(vShaderCode, fShaderCode, gShaderFile != nullptr ? gShaderCode : nullptr);
	return shader;
}

Texture2D ResourceManager::loadTextureFromFile(const char *path, const bool gamma, const bool alpha)
{
	Texture2D texture;
	if (alpha)
	{
		texture.Internal_Format = GL_RGBA;
		texture.Image_Format = GL_RGBA;
	}

	int width, height, nrComponents;
	unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		texture.Generate(width, height, data);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return texture;
}