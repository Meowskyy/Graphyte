#version 330 core

out vec4 FragColor;

in vec2 TexCoords;
in vec4 ParticleColor;

uniform sampler2D mainTexture;
uniform float cutoff;

void main()
{
    FragColor = (texture(mainTexture, TexCoords) * ParticleColor);
	//FragColor = texture(mainTexture, TexCoords);
	
	if (FragColor.a < cutoff)
        // alpha value less than user-specified threshold?
    {
        discard; // yes: discard this fragment
    }
	
}  