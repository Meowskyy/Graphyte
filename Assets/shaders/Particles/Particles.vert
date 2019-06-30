#version 330 core
layout (location = 0) in vec3 vertex; 		// Vertex positions
layout (location = 1) in vec2 texCoords;	// Texture Coordinates
layout (location = 2) in vec4 position;		// Particle positions
layout (location = 3) in vec4 colors;		// Particle colors

out vec2 TexCoords;
out vec4 ParticleColor;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform vec4 tintColor;
uniform float scale;

void main()
{
    TexCoords = texCoords;
    ParticleColor = tintColor * colors;
    gl_Position = projection * view * model * vec4((vertex * position.w * scale) + position.xyz, 1.0);
}