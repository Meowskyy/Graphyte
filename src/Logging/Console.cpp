#include "Console.h"

#include "Resources\ResourceManager.h"
#include "Text\TextRenderer.h"

#include "Input\Input.h"
#include "Rendering\Screen.h"

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
}

void Console::GetCommand() 
{

}
