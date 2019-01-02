#version 400 core

uniform sampler2D texture_diffuse1;

uniform vec3 cameraPosition;

in vec2 TexCoords;

out vec4 FragColor;

float near = 0.1;
float far  = 10.0;

float LinearizeDepth(float depth)
{
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * near * far) / (far + near - z * (far - near));
}

void main(void)
{
	float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
	vec4 color = texture(texture_diffuse1, TexCoords) * vec4(1.0, 1.0, 1.0, 1.0 - vec3(depth));
	FragColor = color;
}
