#pragma once
#include "ECS.h"

namespace Graphyte {
	// The size of one chunk X, Y, Z
	const int CHUNK_SIZE = 32;
	const int WORLD_SIZE_X = 4;
	const int WORLD_SIZE_Y = 4;
	const int WORLD_SIZE_Z = 4;

	class WorldGenerator : public Component {
	private:
		int blocks[(CHUNK_SIZE * WORLD_SIZE_X) * (CHUNK_SIZE * WORLD_SIZE_Y) * (CHUNK_SIZE * WORLD_SIZE_Z)];
	public:
		int seed = 0;

		void OnComponentAdded();

		int GetBlock(int x, int y, int z);
	};
}