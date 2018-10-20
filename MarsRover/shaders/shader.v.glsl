#version 330

//Vertex attributes for position, normals, UV-coordinates.
in vec4 aVertexPosition;
in vec3 aVertexNormal;
in vec2 aTextureCoord;

uniform mat4 Projection;
uniform mat4 ModelView;
uniform mat3 NormalMatrix;
uniform mat4 lightPositionMat;
const vec4 lightPositionV = vec4(0.0,0.0,0.0,1.0);

// The position of a vertex (per-vertex, from the VBO)
layout (location = 0) in vec3 position; 

//We are implementing Blinn Phong lighting,
//this means we must send interpolated normals and vertex positions(without perspecitve),
//to the fragment shader.
out vec2 vTextureCoord;
out vec3 v;
out vec3 N;
out vec3 lightPos;

//The vertex shader is very simple, and serves to pass most of the data
//to the fragment shader.
void main() {


    vTextureCoord = aTextureCoord;
    v = (ModelView * aVertexPosition).xyz;
    N = NormalMatrix * aVertexNormal;
    lightPos = (lightPositionMat * lightPositionV).xyz;
    gl_Position = Projection * ModelView * aVertexPosition;
  }
