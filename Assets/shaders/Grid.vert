#version 400 core

layout (location = 0) in vec3 vertexPos;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;

    gl_Position = projection * view * model * vec4(vertexPos, 1.0);
}
