#version 330	// GLSL version

//*****
// with help from:
// https://learnopengl.com/Lighting/Basic-Lighting
//*****

// Per-fragment color coming from the vertex shader
in vec4 vColor;
in vec3 normal;
in vec3 FragPos;
in vec2 fTexCoords;

// Uniforms
uniform vec3 lightColor = vec3(1.,1.,1.); //white
uniform float ambientStrength = 0.1;
uniform float lightStrength = 2.0;
uniform vec3 lightPos = vec3(0.,5.,-25.);
uniform vec3 viewPos = vec3(0.,0.,0.);
uniform float specularStrength = 1.;

uniform sampler2D diffSampler;
uniform sampler2D normSampler;
uniform sampler2D specSampler;

// Directional light attempt..
//uniform vec3 dirLightDirection = vec3(0.,-1.,0.);
//uniform vec3 dirLightColor = vec3(0.,0.,1.); //intense blue
//uniform float dirLightStrength = 1.;

// Per-frgament output color
out vec4 FragColor;

void main() { 

	//TextureColor


	//Normals and light direction
	vec3 norm = normalize(normal * 2.0 - 1.0);
	vec3 lightDir = normalize(lightPos - FragPos);
	vec3 dir = -vec3(lightDir.x,lightDir.y,lightDir.z);
	vec3 viewDir = normalize(viewPos - FragPos); // for spec purposes

	//Ambient
	vec3 ambient = ambientStrength * lightColor;

	//Diffuse
	float diff = max(dot(norm,dir), 0.);
	vec3 diffuse = (diff * lightColor) * lightStrength;

	//Directional diffuse
//	float dirDiff = max(dot(norm,-dirLightDirection), 0.);
//	vec3 dirDiffuse = (dirDiff * dirLightColor) * dirLightStrength;

	//Specular
	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 56);
	vec3 specular = specularStrength * spec * lightColor;  

	//Color result
//	vec3 result = (ambient + diffuse + dirDiffuse + specular) * vec3(vColor.x, vColor.y, vColor.z);
//	vec3 result = (ambient + diffuse + specular) * vec3(vColor.x, vColor.y, vColor.z);
	vec3 result = (ambient + diffuse + specular) * texture2D(diffSampler, fTexCoords).rgb;
    FragColor = vec4(result,1.);
}