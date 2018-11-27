#version 330

// Vertex attributes (per-vertex, from the VBO)
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in vec4 aTangent;

// transformation and projection matrix to apply to vertices
uniform mat4x4 transformation;
uniform mat4x4 projection;

// = transpose(inverse(transformation))
uniform mat4x4 normal_matrix;

// point-light:
uniform mat4x4 lightPositionMat;
const vec4 lightPositionV = vec4(0.0,15.0,10.0,1.0);

out vec2 vTextureCoord;
out vec3 v; //vertex positison
out vec3 lightPos;
out mat3 v_TBN;

mat3 calculateTBN();

//The vertex shader is very simple, and serves to pass most of the data
//to the fragment shader.
void main() {
	v_TBN =  calculateTBN();

    vTextureCoord = aTexCoords;
    v = (transformation * vec4(aPos,1.)).xyz;
    lightPos = (lightPositionMat * lightPositionV).xyz;
    gl_Position = projection * transformation * vec4(aPos,1.);
  }

  mat3 calculateTBN() {
  	mat3 normalMatrix = mat3(normal_matrix);
	vec3 N = normalize(normalMatrix * aNormal); 
	vec3 T = normalize(normalMatrix * aTangent.xyz);
	vec3 B = normalize(normalMatrix * (cross(aNormal, aTangent.xyz) * aTangent.w) );
	return mat3(T,B,N);
  }
