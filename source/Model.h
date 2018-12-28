#pragma once

#include <string>
#include <vector>

#include "glm\glm.hpp"

//#include "Material.h"
#include "Mesh.h"

// ASSIMP
#include "assimp\Importer.hpp"
#include "assimp\scene.h"
#include "assimp\postprocess.h"

class Model {
public:
	/*  Functions   */
	Model() {};
	Model(std::string const &path, bool gamma = false);

	void Render(Shader shader);
private:
	/*  Model Data */
	std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	std::vector<Mesh> meshes;
	std::string directory;
	bool gammaCorrection;

	/*  Functions   */
	void loadModel(std::string const &path);
	void processNode(aiNode *node, const aiScene *scene);
	Mesh processMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};