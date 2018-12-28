#include "Input.h"

#include <iostream>

#include "Screen.h"

// Redirect callbacks to IMGUI
#include "imgui.h"
#include "imgui_impl_glfw.h"

KeyStates Input::keys[1024];
KeyStates Input::oldKeys[1024];
KeyStates Input::mouseKeys[4];
std::vector<int> Input::changedKeys;

std::map<std::string, float> Input::axis;

double Input::MouseX, Input::MouseY;

int Input::lastX, Input::lastY;

// KEY CALLBACK
#pragma region callbacks
void Input::key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
	if (keys[key] != action) {
		switch (action) {
		case 0: // RELEASED
			keys[key] = Released;
			std::cout << "Key " << key << " state set to 'Released'" << std::endl;
			break;
		case 1: // PRESSED
			keys[key] = Pressed;
			std::cout << "Key " << key << " state set to 'Pressed'" << std::endl;
			break;
		case 2: // DOWN
			keys[key] = Down;
			std::cout << "Key " << key << " state set to 'Down'" << std::endl;
			break;
		}
	}

	if (action != 0)
		changedKeys.push_back(key);

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mode);
	//Graphyte::screen->keyCallbackEvent(key, scancode, action, mode);
}

// glfw: whenever the mouse moves, this callback is called
bool firstMouse = false;
void Input::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	glfwGetCursorPos(window, &MouseX, &MouseY);

#ifdef NANOGUI
	Graphyte::screen->cursorPosCallbackEvent(xpos, ypos);
#endif
}

void Input::mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
	//std::cout << action << std::endl;

	if (mouseKeys[button] != action) {
		switch (action) {
		case 0: // RELEASED
			mouseKeys[button] = Released;
			std::cout << "Mouse button " << button << " state set to 'Released'" << std::endl;
			break;
		case 1: // PRESSED
			mouseKeys[button] = Pressed;
			std::cout << "Mouse button " << button << " state set to 'Pressed'" << std::endl;
			break;
		case 2: // DOWN
			mouseKeys[button] = Down;
			std::cout << "Mouse button " << button << " state set to 'Down'" << std::endl;
			break;
		}
	}
#ifdef NANOGUI
	Graphyte::screen->mouseButtonCallbackEvent(button, action, mods);
#endif
}

void Input::character_callback(GLFWwindow * window, unsigned int codepoint)
{
	std::cout << "KeyCode: " << codepoint << std::endl;
	std::cout << "AsciiChar: " << char(codepoint) << std::endl;
#ifdef NANOGUI
	Graphyte::screen->charCallbackEvent(codepoint);
#endif
}
#pragma endregion callbacks

void Input::UpdateKeyStates() {
	// TODO: Maybe only loop through the keys that have changed
	// Since there is no function to get the length of an array you need to divide it by the type
	for (int key = 0; key < sizeof(keys) / sizeof(KeyStates); key++) {
		// MOUSE BUTTONS
		//std::cout << sizeof(mouseKeys) / sizeof(KeyStates) << std::endl;
		if (key < sizeof(mouseKeys) / sizeof(KeyStates)) {
			if (mouseKeys[key] != NotPressed) {
				switch (mouseKeys[key]) {
				case Pressed:
					mouseKeys[key] = Down;
					std::cout << "Mouse button " << key << " state set to 'Down'" << std::endl;
					break;
				case Released:
					mouseKeys[key] = NotPressed;
					std::cout << "Mouse button " << key << " state set to 'NotPressed'" << std::endl;
					break;
				}
			}
		}
		
		// KEYBOARD BUTTONS
		if (keys[key] != NotPressed) {
			switch (keys[key]) {
			case Pressed:
				keys[key] = Down;
				std::cout << "key " << key << " state set to 'Down'" << std::endl;
				break;
			case Released:
				keys[key] = NotPressed;
				std::cout << "key " << key << " state set to 'NotPressed'" << std::endl;
				break;
			}
		}
	}

	//std::cout << "Looped Through " << sizeof(keys) / sizeof(KeyStates) << " keys" << std::endl;
}

void Input::UpdateMousePosition() {
	axis["Mouse X"] = MouseX - lastX;
	axis["Mouse Y"] = lastY - MouseY; // reversed since y-coordinates go from bottom to top

	lastX = MouseX;
	lastY = MouseY;
}

// TODO: Does not work while the mouse is locked and/or hidden in GLFW
glm::vec2 Input::GetMousePosition() {
	//return glm::vec2(MouseX, Screen::GetWindowSize().y - MouseY);
	return glm::vec2(0, 0);
}

#pragma region Mouse_functions
bool Input::GetMouseButtonPressed(int button)
{
	return mouseKeys[button] == Pressed;
}

bool Input::GetMouseButtonDown(int button)
{
	return mouseKeys[button] == Down;
}

bool Input::GetMouseButtonReleased(int button)
{
	return mouseKeys[button] == Released;
}
#pragma endregion Mouse_functions

#pragma region Key_functions
bool Input::GetKeyPressed(int key) {
	return keys[key] == Pressed;
}

bool Input::GetKeyDown(int key) {
	return keys[key] == Down;
}

bool Input::GetKeyReleased(int key) {
	return keys[key] == Released;
}
#pragma endregion Key_functions

KeyStates Input::GetKeyState(int key) {
	return keys[key];
}

float Input::GetAxis(std::string axisBane) {
	return axis[axisBane];
}
