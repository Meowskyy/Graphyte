#pragma once
/*******************************************************************
** This code is part of Breakout.
**
** Breakout is free software: you can redistribute it and/or modify
** it under the terms of the CC BY 4.0 license as published by
** Creative Commons, either version 4 of the License, or (at your
** option) any later version.
******************************************************************/

#include <map>
#include <string>

#include <glad\glad.h>

#include "Rendering\Material\Material.h"

#include "Math\Math.h"

// A static singleton ResourceManager class that hosts several
// functions to load Textures and Shaders. Each loaded texture
// and/or shader is also stored for future reference by string
// handles. All functions and resources are static and no 
// public constructor is defined.
namespace Graphyte {
	class ResourceManager
	{
	public:
		// Resource storage
		static std::map<std::string, Shader>    Shaders;
		static std::map<std::string, Texture2D> Textures;
		static std::map<std::string, Material> Materials;

		static void UpdateProjection(Matrix4 projection);

		// Loads (and generates) a shader program from file loading vertex, fragment (and geometry) shader's source code. If gShaderFile is not nullptr, it also loads a geometry shader
		static Shader   LoadShader(const GLchar *vertexShaderFile, const GLchar *fragmentShaderFile, const GLchar *geometryShaderFile, const std::string name);
		// Retrieves a stored sader
		static Shader   GetShader(const std::string name);
		// Loads (and generates) a texture from file
		static Texture2D LoadTexture(const GLchar *file, const bool gamma, const bool alpha, const std::string name);
		// Retrieves a stored texture
		static Texture2D GetTexture(const std::string name);

		static Material LoadMaterial(const std::string name);
		static Material& GetMaterial(const std::string name);

		// Properly de-allocates all loaded resources
		static void      Clear();
	private:
		// Private constructor, that is we do not want any actual resource manager objects. Its members and functions should be publicly available (static).
		ResourceManager() { }
		// Loads and generates a shader from file
		static Shader    loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile, const GLchar *gShaderFile = nullptr);
		// Loads a single texture from file
		static Texture2D loadTextureFromFile(const char *path, const bool gamma, const bool alpha);
	};
}