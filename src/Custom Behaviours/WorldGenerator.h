#pragma once
#include "Scripting\BehaviourScript.h"

// The size of one chunk X, Y, Z
const int CHUNK_SIZE = 8;
const int WORLD_SIZE_X = 16;
const int WORLD_SIZE_Y = 16;
const int WORLD_SIZE_Z = 16;

class WorldGenerator : public BehaviourScript {
private:
	int blocks[(CHUNK_SIZE * WORLD_SIZE_X) * (CHUNK_SIZE * WORLD_SIZE_Y) * (CHUNK_SIZE * WORLD_SIZE_Z)];
public:
	int seed = 0;

	void OnBehaviourAdded();

	int GetBlock(int x, int y, int z);
};