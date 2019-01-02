#pragma once

#include <vector>
#include <string>
#include <map>

#include "glm\glm.hpp"

#include "Graphyte\Graphyte.h"

// KeyStates get set as follows > NotPressed > Pressed > Down > Released > Not Pressed
enum KeyStates {
	NotPressed = 0, // When the key is not pressed
	Pressed = 1, // When you just pressed the key
	Down = 2, // When you are holding the key down
	Released = 3 // When the key gets released
};

class Input {
private:
	Input();
	static KeyStates keys[1024];
	static KeyStates oldKeys[1024];
	static KeyStates mouseKeys[4];
	static std::map<std::string, float> axis;

	static int vecSize;
	static std::vector<int> changedKeys;

	static double MouseX, MouseY;
	static int lastX, lastY;

public:
	// CALLBACK FUNCTIONS
	static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);
	static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	static void character_callback(GLFWwindow* window, unsigned int codepoint);

	//KEYSTATE UPDATE
	static void UpdateKeyStates();
	static KeyStates GetKeyState(int key);
	static void UpdateMousePosition();

	static glm::vec2 GetMousePosition();

	static bool GetMouseButtonPressed(int button);
	static bool GetMouseButtonDown(int button);
	static bool GetMouseButtonReleased(int button);

	/// <summary> Gets the KeyState for the pressed key, returns a bool depending on if the button is held Down </summary>
	static bool GetKeyPressed(int key); 
	/// <summary> GetKeyDown TBD </summary>
	static bool GetKeyDown(int key);
	/// <summary> GetKeyReleased TBD </summary>
	static bool GetKeyReleased(int key);


	static float GetAxis(std::string axisName);
};