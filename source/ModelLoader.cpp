#include "ModelLoader.h"

#include <glad/glad.h> 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "MeshRenderer.h"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

#include "Graphyte.h"

void processNode(aiNode *node, const aiScene *scene);

std::string directory;

void ModelLoader::loadGameObject(std::string const &path)
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
	GameObject* loadedObject = new GameObject();
	processNode(scene->mRootNode, scene);
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void processNode(aiNode *node, const aiScene *scene)
{
	GameObject* loadedObject = new GameObject();

	// process each mesh located at the current node
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		std::cout << node->mNumMeshes << std::endl;
		// the node object only contains indices to index the actual objects in the scene. 
		// the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

		loadedObject->transform.name = mesh->mName.C_Str();

		// for each mesh create a gameobject, add a meshrenderer and assign the mesh as meshrenderers mesh
		//loadedObject->transform.children.push_back(new GameObject());
		//loadedObject->transform.children.push_back(new GameObject());
		//loadedObject->transform.children[i].AddBehaviour(new MeshRenderer(loadedObject->transform.children[i]));

		loadedObject->AddBehaviour(new MeshRenderer(loadedObject->transform, mesh, scene, directory));
		//loadedObject.transform.name = mesh->mName;
	}
	// after we've processed all of the meshes (if any) we then recursively process each of the children nodes
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}

	Graphyte::currentScene.gameObjects.push_back(loadedObject);
}
