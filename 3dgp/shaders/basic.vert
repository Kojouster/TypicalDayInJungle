#version 330
#define MAX_BONES 300
uniform mat4 bones[MAX_BONES];


// Uniforms: Transformation Matrices
uniform mat4 matrixProjection;
uniform mat4 matrixView;
uniform mat4 matrixModelView;

// Uniforms: Material Colours
uniform vec3 materialAmbient;
uniform vec3 materialDiffuse;

layout (location = 3) in vec2 aTexCoord;
in vec3 aVertex;
in vec3 aNormal;
layout (location = 4) in vec3 aTangent;
layout (location = 5) in vec3 aBiTangent;




layout (location = 6)in ivec4 aBoneId;		// Bone Ids
layout (location = 7)in  vec4 aBoneWeight;



out vec4 color;
out vec4 position;
out vec3 normal;
out vec2 texCoord0;
out mat3 matrixTangent;


void main(void) 
{
mat4 matrixBone;


	if (aBoneWeight[0] == 0.0)
		matrixBone = mat4(1);
	else
		matrixBone = (bones[aBoneId[0]] * aBoneWeight[0] +
					  bones[aBoneId[1]] * aBoneWeight[1] +
					  bones[aBoneId[2]] * aBoneWeight[2] +
					  bones[aBoneId[3]] * aBoneWeight[3]);





	// calculate position
	position = matrixModelView * matrixBone * vec4(aVertex, 1.0);
    gl_Position = matrixProjection * position;

// calculate normal
normal = normalize(mat3(matrixModelView) * mat3(matrixBone) * aNormal);


// calculate tangent local system transformation
vec3 tangent = normalize(mat3(matrixModelView) * aTangent);
vec3 biTangent = normalize(mat3(matrixModelView) * aBiTangent);
matrixTangent = mat3(tangent, biTangent, normal);


		// calculate texture coordinate
	texCoord0 = aTexCoord;
	

	// calculate light
	color = vec4(0, 0, 0, 1);



}
