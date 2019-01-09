#pragma once

#include <string>

class GameObject;

class ModelLoader {
public:
	static GameObject loadGameObject(std::string const path);
};