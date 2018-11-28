#version 330	// GLSL version

in vec2 fTexCoords;
uniform sampler2D diffSampler;
out vec4 FragColor;

uniform int has_diffuse_map;
uniform vec3 tint_color;

void main() { 
	
	if (has_diffuse_map == 1)
		FragColor = texture2D(diffSampler, fTexCoords);
	
	else FragColor = vec4(tint_color,1);
}