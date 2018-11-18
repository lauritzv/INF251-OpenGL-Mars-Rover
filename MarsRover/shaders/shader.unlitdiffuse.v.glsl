#version 330	// GLSL version

uniform mat4x4 transformation;
uniform mat4x4 projection;

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal; // not used in this shader

out vec2 fTexCoords;

void main() 
{
	fTexCoords = aTexCoords;
	gl_Position = vec4(projection * transformation * vec4(aPos,1.0));
}