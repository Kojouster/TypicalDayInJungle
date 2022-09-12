#version 330

// Input Variables (received from Vertex Shader)
in vec4 color;
in vec4 position;
in vec3 normal;
in vec2 texCoord0;
vec3 normalNew;
in mat3 matrixTangent;


uniform sampler2D texture0;
//Material Colours
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;

uniform sampler2D textureNormal;


// View Matrix

uniform mat4 matrixView;

// Light declarations
struct AMBIENT
{	
	vec3 color;
};
uniform AMBIENT lightAmbient;

struct DIRECTIONAL
{	
	int on;
	vec3 direction;
	vec3 diffuse;
};
uniform DIRECTIONAL lightDir,lightDirNormal;

vec4 AmbientLight(AMBIENT light)
{
	// Calculate Ambient Light
	return vec4(materialAmbient * light.color, 1);
}


vec4 DirectionalLight(DIRECTIONAL light)
{
	// Calculate Directional Light
	vec4 color = vec4(0, 0, 0, 0);
	vec3 L = normalize(mat3(matrixView) * light.direction);
	float NdotL = dot(normal, L);
	if (NdotL > 0)
		color += vec4(materialDiffuse * light.diffuse, 1) * NdotL;
	return color;
}
vec4 DirectionalLightNormal(DIRECTIONAL light)
{
	// Calculate Directional Light
	vec4 color = vec4(0, 0, 0, 0);
	vec3 L = normalize(mat3(matrixView) * light.direction);
	float NdotL = dot(normalNew, L);
	if (NdotL > 0)
		color += vec4(materialDiffuse * light.diffuse, 1) * NdotL;
	return color;


}

// Output Variable (sent down through the Pipeline)
out vec4 outColor;

void main(void) 
{


	outColor = color;

	normalNew = 2.0 * texture(textureNormal, texCoord0).xyz - vec3(1.0, 1.0, 1.0);
	normalNew = normalize(matrixTangent * normalNew);

		outColor += AmbientLight(lightAmbient);
	    outColor += DirectionalLight(lightDir);

		
		if (lightDirNormal.on == 2) 
		outColor += DirectionalLightNormal(lightDirNormal);

	outColor *= texture(texture0, texCoord0);

	



}
