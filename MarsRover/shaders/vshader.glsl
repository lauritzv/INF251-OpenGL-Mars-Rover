#version 330

// The position and normal of a vertex (per-vertex, from the VBO)
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

// transformation and projection matrix to apply to vertices
uniform mat4x4 transformation;
uniform mat4x4 projection;

// = transpose(inverse(transformation))
uniform mat4x4 normal_matrix;

// point-light:
uniform mat4x4 lightPositionMat;
const vec4 lightPositionV = vec4(0.0,10.0,-5.0,1.0);

//We are implementing Blinn Phong lighting,
out vec2 vTextureCoord;
out vec3 v; //vertex positison
out vec3 N; //interpolated normals
out vec3 lightPos;

out vec3 vertexNormal; //raw vertexnormal attrib

//The vertex shader is very simple, and serves to pass most of the data
//to the fragment shader.
void main() {
    vTextureCoord = aTexCoords;
    v = (transformation * vec4(aPos,1.)).xyz;
    N = mat3(normal_matrix) * aNormal;
//	N = mat3(transpose(inverse(transformation))) * aNormal;
	vertexNormal = aNormal;
    lightPos = (lightPositionMat * lightPositionV).xyz;
    gl_Position = projection * transformation * vec4(aPos,1.);
  }
