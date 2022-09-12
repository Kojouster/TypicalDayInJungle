#version 330

// Uniforms: Transformation Matrices
uniform mat4 matrixProjection;
uniform mat4 matrixView;
uniform mat4 matrixModelView;

// Uniforms: Material Colours
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;



in vec3 aVertex;
in vec3 aNormal;
in vec2 aTexCoord;

out vec4 color;
out vec4 position;
out vec3 normal;
out vec2 texCoord0;


out float fogFactor;



// Uniforms: Water Related
uniform float waterLevel;	// water level (in absolute units)
uniform float fogDensity;

// Output: Water Related
out float waterDepth;	// water depth (positive for underwater, negative for the shore)



// Light declarations
struct AMBIENT
{	
	vec3 color;
};
uniform AMBIENT lightAmbient;

struct DIRECTIONAL
{	
	vec3 direction;
	vec3 diffuse;
};
uniform DIRECTIONAL lightDir;

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

void main(void) 
{

	// calculate position
	position = matrixModelView * vec4(aVertex, 1.0);
	gl_Position = matrixProjection * position;

	// calculate normal
	normal = normalize(mat3(matrixModelView) * aNormal);
	
		// calculate depth of water
	waterDepth = waterLevel - aVertex.y;

	  
	

	// calculate texture coordinate
	texCoord0 = aTexCoord;


	// calculate light
	color = vec4(0, 0, 0, 1);
	color += AmbientLight(lightAmbient);
	color += DirectionalLight(lightDir);

		float eyeAlt = dot(-position.xyz, mat3(matrixModelView) * vec3(0, 1, 0));
        fogFactor = exp2(-fogDensity * length(position) * max(waterDepth, 0) /eyeAlt);

		
}
