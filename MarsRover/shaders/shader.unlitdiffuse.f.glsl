#version 330	// GLSL version

in vec2 fTexCoords;
uniform sampler2D diffSampler;
out vec4 FragColor;

void main() { 
	FragColor = texture2D(diffSampler, fTexCoords);
}