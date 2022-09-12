#version 330

// Input Variables (received from Vertex Shader)
in vec4 color;
in vec4 position;
in vec3 normal;
in vec2 texCoord0;

// Output Variable (sent down through the Pipeline)
out vec4 outColor;


uniform vec3 waterColor;

// Input: Water Related
in float waterDepth;	

void main(void) 
{
	outColor = color;
}
