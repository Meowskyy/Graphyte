#pragma once

#include <string>

class Console {
private:
	static std::string inputText;

	static void GetCommand();
public:
	static void Update();
};