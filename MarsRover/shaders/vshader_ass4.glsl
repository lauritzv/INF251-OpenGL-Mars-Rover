#version 330

// Vertex attributes (per-vertex, from the VBO)
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec4 aTangent; // These are currently not being passed correctly
//layout (location = 4) in vec3 aBiTangent;

// transformation and projection matrix to apply to vertices
uniform mat4x4 transformation;
uniform mat4x4 projection;

// = transpose(inverse(transformation))
uniform mat4x4 normal_matrix;

bool mirrorY = false;
uniform int hasDiffuseMap;
uniform int hasNormalMap;
uniform int hasSpecularMap;

out vec2 vTextureCoord;
out vec3 v; //vertex positison
out mat3 v_TBN;

flat out int hasDiffuseMapV;
flat out int hasNormalMapV;
flat out int hasSpecularMapV;

mat3 calculateTBN();

// The vertex shader mostly serves to pass data to the fragment shader.
void main() 
{
	hasDiffuseMapV = hasDiffuseMap;
	hasNormalMapV = hasNormalMap;
	hasSpecularMapV = hasSpecularMap;

	if (mirrorY && hasDiffuseMap + hasNormalMap + hasSpecularMap > 0)
		vTextureCoord.y = 1. - aTexCoords.y;

	v_TBN =  calculateTBN();

    vTextureCoord = aTexCoords;
    v = (transformation * vec4(aPos,1.)).xyz;

    gl_Position = projection * transformation * vec4(aPos,1.);
}

mat3 calculateTBN() 
{
	mat3 normalMatrix = mat3(normal_matrix);
	vec3 N = normalize(normalMatrix * aNormal); 
	vec3 T = normalize(normalMatrix * aTangent.xyz);
	vec3 B = normalize(normalMatrix * (cross(aNormal, aTangent.xyz) * aTangent.w) );
	return mat3(T,B,N);
}
