#include "Custom Behaviours\WorldGenerator.h"
#include "Custom Behaviours\Chunk.h"

#include "Math\PerlinNoise.h"

void WorldGenerator::OnComponentAdded() {
	PerlinNoise perlinNoise;

	float noiseScale = 20;

	for (int x = 0; x < CHUNK_SIZE * WORLD_SIZE_X; x++) {
		for (int z = 0; z < CHUNK_SIZE * WORLD_SIZE_Z; z++) {
			for (int y = 0; y < CHUNK_SIZE * WORLD_SIZE_Y; y++) {
				int index = x + y * (CHUNK_SIZE * WORLD_SIZE_Y) + z * (CHUNK_SIZE * WORLD_SIZE_X) * (CHUNK_SIZE * WORLD_SIZE_Y);

				if (perlinNoise.noise0_1(x / noiseScale, y / noiseScale, z / noiseScale) > 0.4) {
					blocks[index] = 1;
				}
				else {
					blocks[index] = 0;
				}
			}
		}
	}

	std::cout << "Added VoxelGenerator" << std::endl;

	// Creates a chunk
	for (int y = 0; y < WORLD_SIZE_Y; y++) {
		for (int z = 0; z < WORLD_SIZE_Z; z++) {
			for (int x = 0; x < WORLD_SIZE_X; x++) {

				GameObject *object = Scene::Instantiate(new GameObject(), gameObject);
				object->transform.name = std::to_string(x * CHUNK_SIZE) + ", " + std::to_string(y * CHUNK_SIZE) + ", " + std::to_string(z * CHUNK_SIZE);
				object->transform.position = Vector3(x * CHUNK_SIZE, y * CHUNK_SIZE, z * CHUNK_SIZE);
			
				Chunk* voxelGen = &object->AddComponent<Chunk>();
				voxelGen->chunkX = x * CHUNK_SIZE;
				voxelGen->chunkY = y * CHUNK_SIZE;
				voxelGen->chunkZ = z * CHUNK_SIZE;
				voxelGen->world = this;

				voxelGen->InitializeChunk();
			}
		}
	}
}

int WorldGenerator::GetBlock(int x, int y, int z) {
	if (x >= CHUNK_SIZE * WORLD_SIZE_X || x < 0 || y >= CHUNK_SIZE * WORLD_SIZE_Y || y < 0 || z >= CHUNK_SIZE * WORLD_SIZE_Z || z < 0)
	{
		return 1;
	}
	
	int index = x + y * (CHUNK_SIZE * WORLD_SIZE_Y) + z * (CHUNK_SIZE * WORLD_SIZE_X) * (CHUNK_SIZE * WORLD_SIZE_Y);
	return blocks[index];
}
