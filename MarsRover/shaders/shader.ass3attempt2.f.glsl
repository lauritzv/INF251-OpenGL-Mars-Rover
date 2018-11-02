#version 330	// GLSL version

//*****
// with help from:
// https://learnopengl.com/Lighting/Basic-Lighting
//*****

// Per-fragment color coming from the vertex shader
//in vec4 vColor;
in vec3 normal;
in vec3 FragPos;
in vec2 fTexCoords;

in vec3 d_light_direction;

// Uniforms
uniform vec3 d_light_a_color = vec3(1.,1.,1.); //white
uniform float d_light_a_intensity = 0.1; //amblight str.
uniform float d_light_d_intensity = 1.0; //dirlight str.
uniform vec3 camera_position;
uniform float d_light_s_intensity = 1.;

uniform sampler2D diffSampler;
uniform sampler2D normSampler;
uniform sampler2D specSampler;

uniform float material_shininess = 56.;

// Directional light attempt..
//uniform vec3 dirLightDirection = vec3(0.,-1.,0.);
//uniform vec3 dirLightColor = vec3(0.,0.,1.); //intense blue
//uniform float dirLightStrength = 1.;

// Per-frgament output color
out vec4 FragColor;

void main() { 

	vec3 position = FragPos;

	//TextureColors
	vec3 material_d_color = texture2D(diffSampler, fTexCoords).rgb;
	vec3 material_a_color = material_d_color;
	vec3 normTexColor = texture2D(normSampler, fTexCoords).rgb;
	vec3 material_s_color = texture2D(specSampler, fTexCoords).rgb;
//	vec3 material_s_color = vec3(1.,1.,1.);
//
//	//Normals and light direction
//	vec3 norm = normalize(normal * 2.0 - 1.0);
////	vec3 norm = normalize(normal);
//	vec3 d_light_dir_nn = normalize(d_light_direction);
//	vec3 dir = vec3(d_light_dir_nn.x,d_light_dir_nn.y,d_light_dir_nn.z);
//	vec3 view_dir_nn = normalize(camera_position - position); // for spec purposes
//
//	float dot_d_light_normal = dot(-d_light_dir_nn, normal);   // notice the minus!
//	vec3 d_reflected_dir_nn = d_light_dir_nn + 2. * dot_d_light_normal * normal;
//	// should be already normalized, but we "need" to correct numerical errors
//	d_reflected_dir_nn = normalize(d_reflected_dir_nn); 
//	
//
//	// compute the color contribution	
//	vec3 color;
//	vec3 amb_color = clamp( material_d_color * d_light_a_color * d_light_a_intensity, 0.0, 1.0);
//	vec3 diff_color = clamp( material_d_color * dot_d_light_normal * d_light_d_intensity, 0.0, 1.0);
//	vec3 spec_color = clamp( material_s_color * pow(dot(d_reflected_dir_nn, view_dir_nn), material_shininess), 0.0, 1.0);
//	color = clamp( amb_color + diff_color + spec_color, 0.0, 1.0);
	// --- directional light ----
	// compute the required values and vectors
	// notice that input variables cannot be modified, so copy them first
	vec3 normal_nn = normalize(normal);
//	vec3 normal_nn = normalize(normTexColor * normal);
	vec3 d_light_dir_nn = normalize(d_light_direction);
	vec3 view_dir_nn = normalize(camera_position - position);
	//d_light_dir_nn = view_dir_nn;
	
	float dot_d_light_normal = dot(-d_light_dir_nn, normal_nn);   // notice the minus!
	vec3 d_reflected_dir_nn = d_light_dir_nn + 2. * dot_d_light_normal * normal_nn;
	// should be already normalized, but we "need" to correct numerical errors
	d_reflected_dir_nn = normalize(d_reflected_dir_nn);
	
	// compute the color contribution	
	vec3 color;
	vec3 amb_color = clamp(
			material_a_color * d_light_a_color * d_light_a_intensity,
			0.0, 1.0);
//	vec3 amb_color = vec3(0.,0.,0.);
	vec3 diff_color = 
	clamp(	material_d_color * dot_d_light_normal * d_light_d_intensity,
			0.0, 1.0);
//	vec3 spec_color = vec3(0.,0.,0.);
	vec3 spec_color = 
	clamp(
		material_s_color *  
		pow(dot(d_reflected_dir_nn, view_dir_nn), material_shininess),
		0.0, 1.0);

	vec3 vp = FragPos;
    //Light direction and vertex directions.
    vec3 L = normalize(d_light_dir_nn);
    vec3 E = normalize(-vp);

    //****BLINN PHONG****//
//    float lambertian = max(dot(L,normal_nn), 0.0);
	float lambertian = dot_d_light_normal;
//    float specular = 0.0;
//    if(lambertian > 0.0) {
//        vec3 halfDir = normalize(L + E);
//        float specAngle = max(dot(halfDir, normal_nn), 0.0);
//        specular = pow(specAngle, material_shininess);
//    }

	float specEnabled = 0.;
	if (lambertian > 0.){
		specEnabled = 1.;
	}

	color = clamp(amb_color + lambertian * diff_color + specEnabled * spec_color,0.0, 1.0);
//		
//	if (lambertian > 0.){
//			color = clamp(color + spec_color, 0., 1.);
//	}
//
	// TODO: do the same for the headlight!
	// notice that for the headlight dot(view_dir, light_dir) = ...
	//p_light_dir_nn = view_dir_nn;
	
//	vec3 p_light_dir_nn = -view_dir_nn;
//	
//	float dot_p_light_normal = dot(-p_light_dir_nn, normal);   // notice the minus!
//	vec3 p_reflected_dir_nn = d_light_dir_nn + 2. * dot_d_light_normal * normal;
//	// should be already normalized, but we "need" to correct numerical errors
//	d_reflected_dir_nn = normalize(d_reflected_dir_nn); 
//	
//	// compute the color contribution	
//	amb_color = clamp( material_d_color * d_light_a_color * d_light_a_intensity, 0.0, 1.0);
//	diff_color = clamp( material_d_color * dot_p_light_normal * d_light_d_intensity, 0.0, 1.0);
//	spec_color = clamp( material_s_color * pow(dot(d_reflected_dir_nn, view_dir_nn), material_shininess), 0.0, 1.0);
//	color = clamp( amb_color + diff_color + spec_color, 0.0, 1.0);
//	color = clamp(color, 0.0, 1.0);
		
	// pass the reuslt to the fragment shader
	FragColor = vec4(color, 1.0);
//	FragColor = vec4(clamp(normal,0.,1.),1.0);


//	//Ambient
//	vec3 ambient = ambientStrength * lightColor;
//
//	//Diffuse
//	float diff = max(dot(norm,dir), 0.);
//	vec3 diffuse = (diff * lightColor) * lightStrength;
//
//	//Directional diffuse
////	float dirDiff = max(dot(norm,-dirLightDirection), 0.);
////	vec3 dirDiffuse = (dirDiff * dirLightColor) * dirLightStrength;
//
//	//Specular
//	vec3 reflectDir = reflect(lightDir, norm);
//	float spec = pow(max(dot(viewDir_nn, reflectDir), 0.0), 56);
//	vec3 specular = specularStrength * spec * lightColor;  
//
//	//Color result
////	vec3 result = (ambient + diffuse + dirDiffuse + specular) * vec3(vColor.x, vColor.y, vColor.z);
////	vec3 result = (ambient + diffuse + specular) * vec3(vColor.x, vColor.y, vColor.z);
//	vec3 result = (ambient + diffuse + specular) * diffTexColor;
//    FragColor = vec4(result,1.);


//bump: ws_pos += bmp * normal
}