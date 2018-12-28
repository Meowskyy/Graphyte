#pragma once

#include "Shader.h"
#include <map>

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Material {
public:
	Shader shader;
	std::map<std::string, Texture> textures;

	// Constructor
	Material();

	// Func
	void use();
};