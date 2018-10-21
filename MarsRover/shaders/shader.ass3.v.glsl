#version 330	// GLSL version

//*****
// with help from:
// https://learnopengl.com/Lighting/Basic-Lighting
//*****

// transformation and projection matrix to apply to vertices
uniform mat4x4 transformation;
uniform mat4x4 projection;

// The position and normal of a vertex (per-vertex, from the VBO)
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 4) in vec2 aTexCoords;

// Output vertex color (per-vertex, interpolated and passed to frag shader)
out vec4 vColor;
out vec3 normal;
out vec3 FragPos;

void main() {
	
	//Fragment's non-projected position
	FragPos = vec3(transformation * vec4(aPos,1.0));

	//Fragment's projected position
	gl_Position = vec4(projection * transformation * vec4(aPos,1.0));

	// set the color of the vertex
//	vColor = vec4(0.5,0.5,0.5,1.0) + 0.2*vec4(normalize(aNormal),1.); // grey with a slight tint from normals
//	vColor = vec4(normalize(aNormal),1.); // grey with a slight tint from normals
	vColor = vec4(normalize(aTexCoords), 0, 1);
	normal = aNormal;
//	normal = mat3(transpose(inverse(transformation))) * aNormal;
}