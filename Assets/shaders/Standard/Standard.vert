#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out VS_OUT {
    vec3 Normal;
    vec2 TexCoords;
	vec4 LightSpacePos;
	vec3 WorldPos;
} vs_out;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;
uniform mat4 lightSpaceMatrix;

void main()
{	
    gl_Position = projection * view * model * vec4(position, 1.0);
	vs_out.LightSpacePos = lightSpaceMatrix * vec4(position, 1.0);
	vs_out.TexCoords = texCoords;
	vs_out.Normal = mat3(transpose(inverse(model))) * normal;	
	//vs_out.Normal = (model * vec4(normal, 0.0)).xyz;
	vs_out.WorldPos = (model * vec4(position, 1.0)).xyz;
}