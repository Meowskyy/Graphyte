#pragma once

#include "Rendering\Material\Shader.h"
#include "Rendering\Material\Texture.h"
#include <map>
#include <vector>

struct Texture {
	unsigned int id;
	std::string type;
	std::string path;
};

class Material {
public:
	Shader shader;
	std::vector<Texture2D> textures;

	// Constructor
	Material();

	// Func
	void Use();
};