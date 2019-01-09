#include "ModelLoader.h"

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Rendering\Mesh.h"
#include "Rendering\MeshRenderer.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "Graphyte\Graphyte.h"

#include "Objects\Scene.h"

GameObject processNode(aiNode* node, const aiScene* scene);

std::string directory;

GameObject ModelLoader::loadGameObject(const std::string path)
{
	// read file via ASSIMP
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
	// check for errors
	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
	{
		std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
	}
	// retrieve the directory path of the filepath
	directory = path.substr(0, path.find_last_of('/'));

	// process ASSIMP's root node recursively
	GameObject gameObject = GameObject();
	gameObject.AddChild(processNode(scene->mRootNode, scene));

	return gameObject;
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
GameObject processNode(aiNode* node, const aiScene* scene)
{
	GameObject gameObject = GameObject();

	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		// For each mesh add a new child object
		gameObject.AddChild(GameObject());
		GameObject* loadedObject = gameObject.GetChild(i);
		//GameObject* loadedObject = Scene::Instantiate(new GameObject());

		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

		std::string name = mesh->mName.C_Str();

		loadedObject->transform.name = name;
		loadedObject->AddBehaviour(new MeshRenderer(mesh, scene, directory));
	}

	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		GameObject* child = gameObject.GetChild(i);
			child->AddChild(processNode(node->mChildren[i], scene));
	}

	return gameObject;
}
