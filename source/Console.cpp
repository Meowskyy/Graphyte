#include "Console.h"

#include "ResourceManager.h"
#include "TextRenderer.h"

#include "Input.h"
#include "Screen.h"

std::string Console::inputText;

void Console::Update()
{

	if (Input::GetKeyPressed(GLFW_KEY_ENTER)) 
	{
		GetCommand();
		inputText = "";
	}

	if (Input::GetKeyPressed(GLFW_KEY_TAB)) 
	{

	}

	if (Input::GetKeyPressed(GLFW_KEY_BACKSPACE)) {
		inputText = inputText.substr(0, inputText.size() - 1);
	}

	//TextRenderer::RenderText(ResourceManager::GetShader("GUIText"), inputText, 25.0f, Screen::GetWindowSize().y - 50, 1.0f, glm::vec3(1.0f, 1.0f, 1.0f));

}

void Console::GetCommand() 
{

}
