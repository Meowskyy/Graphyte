#pragma once

//#include "GL\glew.h"
#include "glad\glad.h"
#include "GLFW\glfw3.h"

#include <map>

#include "Rendering\Material\Shader.h"

/// Holds all state information relevant to a character as loaded using FreeType
struct Character {
	GLuint TextureID;   // ID handle of the glyph texture
	glm::ivec2 Size;    // Size of glyph
	glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
	GLuint Advance;    // Horizontal offset to advance to next glyph
};

class TextRenderer {
private:
	static std::map<GLchar, Character> Characters;
	static GLuint VAO, VBO;

	TextRenderer();
public:
	static void Init();

	static void RenderText(Shader &newShader, std::string text, GLfloat x, GLfloat y, GLfloat scale, glm::vec3 color);
};