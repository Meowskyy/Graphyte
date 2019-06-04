#pragma once

#include "Rendering\Material\Shader.h"
#include "Rendering\Material\Texture.h"
#include <map>
#include <vector>

namespace Graphyte {
	struct Texture {
		unsigned int id;
		std::string type;
		std::string path;
	};

	class Material {
	public:
		std::string name;
		Shader shader;
		std::vector<Texture2D> textures;

		// Constructor
		Material();

		// Func
		void Use();
	};
}