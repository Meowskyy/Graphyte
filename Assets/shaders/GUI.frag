#version 330 core

out vec4 FragColor;

uniform vec3 guiColor;

void main()
{
	FragColor = vec4(guiColor, 1.0f);
}
