#version 330	// GLSL version

//*****
// with help from:
// https://learnopengl.com/Lighting/Basic-Lighting
//*****

// transformation and projection matrix to apply to vertices
uniform mat4x4 transformation;
uniform mat4x4 projection;
uniform mat4x4 normal_matrix;
// The position and normal of a vertex (per-vertex, from the VBO)
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;
layout (location = 2) in vec3 aNormal;

uniform vec3 LightPosition_worldspace = vec3(0.,5.,0.);

// Output vertex color (per-vertex, interpolated and passed to frag shader)
//out vec4 vColor;
out vec3 normal;
out vec3 FragPos;
out vec2 fTexCoords;
out vec3 d_light_direction;

//out vec3 Position_worldspace;
//out vec3 EyeDirection_cameraspace;
//out vec3 LightDirection_cameraspace;
//
//out vec3 LightDirection_tangentspace;
//out vec3 EyeDirection_tangentspace;
//
void main() {
	
	d_light_direction = normalize(vec3(0.,0.,0.) - LightPosition_worldspace);

	//Fragment's non-projected position
	FragPos = vec3(transformation * vec4(aPos,1.0));
	fTexCoords = aTexCoords;

	//Fragment's projected position
	gl_Position = vec4(projection * vec4(FragPos,1.));
	
//	EyeDirection_cameraspace = vec3(0,0,0) - gl_Position.xyz;
//	vec3 LightPosition_cameraspace = ( projection * vec4(d_light_position,1)).xyz;
//	vec3 LightDirection_cameraspace = LightPosition_cameraspace + EyeDirection_cameraspace;
//
	// set the color of the vertex
//	vColor = vec4(0.5,0.5,0.5,1.0) + 0.2*vec4(normalize(aNormal),1.); // grey with a slight tint from normals
//	vColor = vec4(normalize(aNormal),1.); //normal colors
//	vColor = vec4(normalize(aTexCoords), 0, 1);
	normal = normalize(mat3(normal_matrix) * aNormal);
//	normal = mat3(transpose(inverse(transformation))) * aNormal;
}