#version 330

// Input Variables (received from Vertex Shader)
in vec4 color;
in vec4 position;
in vec3 normal;
in vec2 texCoord0;
// Water-related

uniform vec4 waterColor;
uniform vec4 skyColor;

in float reflFactor;			// reflection coefficient



// Output Variable (sent down through the Pipeline)
out vec4 outColor;

void main(void) 
{
	outColor = color;


	outColor = mix(waterColor, skyColor, reflFactor);
}
