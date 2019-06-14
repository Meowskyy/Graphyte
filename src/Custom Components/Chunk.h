#pragma once

#include "ECS.h"

#include "Time\Timers.h"
#include "Graphyte\Graphyte.h"
#include "Rendering\MeshRenderer.h"
#include "Custom Components\WorldGenerator.h"

#include "Physics\Collider.h"

#include <math.h>

namespace Graphyte {
	class Chunk : public Component {
	private:
		std::vector<Vector3> vertices;
		std::vector<Vector2> uvs;
		std::vector<unsigned int> indices;
		int faceCount = 0;

		MeshRenderer* meshRenderer;
		Mesh* mesh;

		Vector2 tStone = Vector2(0, 0);
		float tUnit = 1.0f;
	public:
		WorldGenerator* world;

		int chunkX, chunkY, chunkZ;

		bool update = false;

		void OnComponentAdded()
		{
			meshRenderer = &gameObject->AddComponent<MeshRenderer>();
			mesh = &meshRenderer->mesh;

			meshRenderer->SetMaterial(ResourceManager::GetMaterial("TerrainMaterial"));
			//meshRenderer->SetMaterial(ResourceManager::GetMaterial("Standard"));

			gameObject->AddComponent<Collider>();
		}

		void InitializeChunk()
		{
			vertices.clear();
			uvs.clear();
			indices.clear();

			for (int y = 0; y < CHUNK_SIZE; y++) {
				for (int z = 0; z < CHUNK_SIZE; z++) {
					for (int x = 0; x < CHUNK_SIZE; x++) {

						// IF BLOCK IS NOT AIR
						if (world->GetBlock(chunkX + x, chunkY + y, chunkZ + z) != 0) {
							// IF NO BLOCK ABOVE CREATE PLANE_TOP
							if (world->GetBlock(chunkX + x, chunkY + y + 1, chunkZ + z) == 0)
								CubeTop(Vector3(x, y, z));
							// IF NO BLOCK BELOW CREATE PLANE_BOTTOM
							if (world->GetBlock(chunkX + x, chunkY + y - 1, chunkZ + z) == 0)
								CubeBottom(Vector3(x, y, z));

							// IF NO BLOCK NORTH CREATE NORTH
							if (world->GetBlock(chunkX + x, chunkY + y, chunkZ + z + 1) == 0)
								CubeNorth(Vector3(x, y, z));
							// IF NO BLOCK SOUTH CREATE SOUTH
							if (world->GetBlock(chunkX + x, chunkY + y, chunkZ + z - 1) == 0)
								CubeSouth(Vector3(x, y, z));

							// IF NO BLOCK EAST CREATE EAST
							if (world->GetBlock(chunkX + x + 1, chunkY + y, chunkZ + z) == 0)
								CubeEast(Vector3(x, y, z));
							// IF NO BLOCK WEST CREATE WEST
							if (world->GetBlock(chunkX + x - 1, chunkY + y, chunkZ + z) == 0)
								CubeWest(Vector3(x, y, z));
						}

					}
				}
			}

			UpdateMesh();
		}

		void Update() {
			if (update) {
				InitializeChunk();
				UpdateMesh();
				update = false;
			}
		}

		void UpdateMesh()
		{
			mesh->vertices = vertices;
			mesh->indices = indices;
			mesh->uvs = uvs;
			mesh->RecalculateNormals();
			mesh->SetupMesh();


			meshRenderer->RecalculateBoundingBox();
		}

		void CubeTop(Vector3 position) {
			vertices.push_back(Vector3(position.x, position.y, position.z + 1));
			vertices.push_back(Vector3(position.x + 1, position.y, position.z + 1));
			vertices.push_back(Vector3(position.x + 1, position.y, position.z));
			vertices.push_back(Vector3(position.x, position.y, position.z));

			CreateTriangles();
		}

		void CubeBottom(Vector3 position) {
			vertices.push_back(Vector3(position.x, position.y - 1, position.z));
			vertices.push_back(Vector3(position.x + 1, position.y - 1, position.z));
			vertices.push_back(Vector3(position.x + 1, position.y - 1, position.z + 1));
			vertices.push_back(Vector3(position.x, position.y - 1, position.z + 1));

			CreateTriangles();
		}

		void CubeNorth(Vector3 position) {
			vertices.push_back(Vector3(position.x + 1, position.y - 1, position.z + 1));
			vertices.push_back(Vector3(position.x + 1, position.y, position.z + 1));
			vertices.push_back(Vector3(position.x, position.y, position.z + 1));
			vertices.push_back(Vector3(position.x, position.y - 1, position.z + 1));

			CreateTriangles();
		}

		void CubeSouth(Vector3 position) {
			vertices.push_back(Vector3(position.x, position.y - 1, position.z));
			vertices.push_back(Vector3(position.x, position.y, position.z));
			vertices.push_back(Vector3(position.x + 1, position.y, position.z));
			vertices.push_back(Vector3(position.x + 1, position.y - 1, position.z));

			CreateTriangles();
		}

		void CubeWest(Vector3 position) {
			vertices.push_back(Vector3(position.x, position.y - 1, position.z + 1));
			vertices.push_back(Vector3(position.x, position.y, position.z + 1));
			vertices.push_back(Vector3(position.x, position.y, position.z));
			vertices.push_back(Vector3(position.x, position.y - 1, position.z));

			CreateTriangles();
		}

		void CubeEast(Vector3 position) {
			vertices.push_back(Vector3(position.x + 1, position.y - 1, position.z));
			vertices.push_back(Vector3(position.x + 1, position.y, position.z));
			vertices.push_back(Vector3(position.x + 1, position.y, position.z + 1));
			vertices.push_back(Vector3(position.x + 1, position.y - 1, position.z + 1));

			CreateTriangles();
		}

		void CreateTriangles() {
			indices.push_back(faceCount * 4);		//1
			indices.push_back(faceCount * 4 + 1);	//2
			indices.push_back(faceCount * 4 + 2);	//3
			indices.push_back(faceCount * 4);		//1
			indices.push_back(faceCount * 4 + 2);	//3
			indices.push_back(faceCount * 4 + 3);	//4

			Vector2 texturePos = tStone;

			uvs.push_back(Vector2(tUnit * texturePos.x + tUnit, tUnit * texturePos.y));
			uvs.push_back(Vector2(tUnit * texturePos.x + tUnit, tUnit * texturePos.y + tUnit));
			uvs.push_back(Vector2(tUnit * texturePos.x, tUnit * texturePos.y + tUnit));
			uvs.push_back(Vector2(tUnit * texturePos.x, tUnit * texturePos.y));

			faceCount++;
		}
	};
}