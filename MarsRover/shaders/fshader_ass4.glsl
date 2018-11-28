#version 330

precision mediump float;

in vec2 vTextureCoord;
in vec3 v;
in vec3 lightPos;
in mat3 v_TBN;

uniform sampler2D diffSampler;
uniform sampler2D normSampler;
uniform sampler2D specSampler;

uniform int hasDiffuseMap;
uniform int hasNormalMap;
uniform int hasSpecularMap;

out vec4 fColor;

uniform vec3 specularColor;
uniform vec3 diffuseTint;

const float shinyness = 32.;
const float ambientStrength = .1;
	
vec3 GetNormal();
vec4 GetDiffuseColor();

void main() {
    
    //Sample the texture into textureColor.
    //Then we mix it with our ambient, diffuse, and specular colors.

	vec4 diffuse = GetDiffuseColor();
	vec4 ambient = ambientStrength * diffuse;

    vec4 specularcolor = vec4(specularColor,1.);
	if (hasSpecularMap == 1)
		specularcolor *= texture(specSampler, vTextureCoord);
    
	//Calculate the normal
    vec3 normal = GetNormal();
	
	vec3 lp = lightPos;
    vec3 vp = v;

	//Move light direction and vertex direction to tangent space
//	lp = v_TBN * lightPos;
//    vp = v_TBN * v;

    //Light direction and vertex directions.
    vec3 L = normalize(lp - vp);
    vec3 E = normalize(-vp);

    //****BLINN PHONG****//
    float lambertian = max(dot(L,normal), 0.0);
    float specular = 0.;

    if(lambertian > 0.) {
        vec3 halfDir = normalize(L + E);
        float specAngle = max(dot(halfDir, normal), 0.);
        specular = pow(specAngle, shinyness);
    }
    
    //Output final color
	fColor = ambient + lambertian * diffuse + specular * specularcolor;
}

vec3 GetNormal(){
    vec3 n;
	if (hasNormalMap == 1)
		n = texture(normSampler,vTextureCoord).xyz;
	else n = vec3(.5,.5,1.);
    
	return normalize(v_TBN * n);
}

vec4 GetDiffuseColor()
{
	vec4 textureColor;
	if (hasDiffuseMap == 1)
		textureColor = texture(diffSampler, vTextureCoord);
    else
		textureColor = vec4(diffuseTint,1.);

	return textureColor;
}
