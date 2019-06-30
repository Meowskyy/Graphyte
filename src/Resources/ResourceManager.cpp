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

Shader     ResourceManager::currentShader;

void Graphyte::ResourceManager::LoadDefaults()
{
	ResourceManager::LoadShaders();
	ResourceManager::LoadTextures();
	ResourceManager::LoadMaterials();
	ResourceManager::LoadModels();
}

// TODO: Moving these somewhere more appropriate
void ResourceManager::LoadShaders()
{
	// EDITOR STUFF
	ResourceManager::LoadShader("Assets/shaders/Grid.vert", "Assets/shaders/Grid.frag", nullptr, "Grid");
	//ResourceManager::LoadShader("Assets/shaders/SimpleDepth.vert", "Assets/shaders/SimpleDepth.frag", nullptr, "SimpleDepth");
	ResourceManager::LoadShader("Assets/shaders/DepthDebug.vert", "Assets/shaders/DepthDebug.frag", nullptr, "DepthDebug");
	ResourceManager::LoadShader("Assets/shaders/ShadowMap.vert", "Assets/shaders/ShadowMap.frag", nullptr, "ShadowMap");
	ResourceManager::LoadShader("Assets/shaders/Editor/ScreenShader.vert", "Assets/shaders/Editor/ScreenShader.frag", nullptr, "ScreenShader");

	// DEFAULT SHADERS
	//ResourceManager::LoadShader("Assets/shaders/PBR.vert", "Assets/shaders/PBR.frag", nullptr, "Standard");
	ResourceManager::LoadShader("Assets/shaders/Standard/StandardV2.vert", "Assets/shaders/Standard/StandardV2.frag", nullptr, "Standard");
	ResourceManager::LoadShader("Assets/shaders/Unlit.vert", "Assets/shaders/Unlit.frag", nullptr, "Unlit");
	//ResourceManager::LoadShader("Assets/shaders/Particles/Particles.vert", "Assets/shaders/Particles/Particles.frag", "Assets/shaders/Particles/Particles.geom", "Particles");
	ResourceManager::LoadShader("Assets/shaders/Particles/Particles.vert", "Assets/shaders/Particles/Particles.frag", nullptr, "Particles");

	//ResourceManager::LoadShader("Assets/shaders/Lighting.vert", "Assets/shaders/Lighting.frag", nullptr, "Lighting");
	//ResourceManager::LoadShader("Assets/shaders/ScreenShader.vert", "Assets/shaders/ScreenShader.frag", nullptr, "ScreenShader");
}

// TODO: Moving these somewhere more appropriate
void ResourceManager::LoadTextures()
{
	ResourceManager::LoadTexture("Assets/textures/Grid/texture_diffuse.psd", false, true, "Grid");

	ResourceManager::LoadTexture("Assets/textures/GrassTexture.psd", false, false, "GrassTexture");
	ResourceManager::GetTexture("GrassTexture").SetFiltering(false);

	ResourceManager::LoadTexture("Assets/textures/particle.psd", false, true, "Particle");
}

// TODO: Moving these somewhere more appropriate
void ResourceManager::LoadMaterials()
{
	ResourceManager::LoadMaterial("GridMaterial");
	ResourceManager::GetMaterial("GridMaterial").textures.push_back(ResourceManager::GetTexture("Grid"));
	ResourceManager::GetMaterial("GridMaterial").shader = ResourceManager::GetShader("Grid");

	ResourceManager::LoadMaterial("TerrainMaterial");
	ResourceManager::GetMaterial("TerrainMaterial").textures.push_back(ResourceManager::GetTexture("GrassTexture"));

	ResourceManager::LoadMaterial("LightingMat");
	ResourceManager::GetMaterial("LightingMat").shader = ResourceManager::GetShader("Lighting");

	ResourceManager::LoadMaterial("Particles");
	ResourceManager::GetMaterial("Particles").shader = ResourceManager::GetShader("Particles");
	ResourceManager::GetMaterial("Particles").textures.push_back(ResourceManager::GetTexture("Particle"));
}

// TODO: Moving these somewhere more appropriate
void ResourceManager::LoadModels()
{
	//test = Model("Assets/resources/nanosuit/nanosuit.obj");

	//TextRenderer::Init();
	//TextRenderer::SetShader(ResourceManager::GetShader("GUIText"));
}

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