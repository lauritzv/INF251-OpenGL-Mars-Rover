#version 330

precision mediump float;

in vec2 vTextureCoord;
in vec3 v;
in vec3 lightPos;
in mat3 v_TBN;

uniform sampler2D diffSampler;
uniform sampler2D normSampler;
uniform sampler2D specSampler;

flat in int hasDiffuseMapV;
flat in int hasNormalMapV;
flat in int hasSpecularMapV;

out vec4 fColor;

uniform vec3 specularColor;
uniform vec3 diffuseTint;

const float shinyness = 75.;
const float ambientStrength = .2;
	
vec3 GetNormal();
vec4 GetDiffuseColor();
//vec2 newTexCoord;
bool flipRG = true;

void main() 
{
    //Sample the texture into textureColor.
    //Then we mix it with our ambient, diffuse, and specular colors.
	vec4 diffuse = GetDiffuseColor();
	vec4 ambient = ambientStrength * diffuse;

    vec4 specularcolor = vec4(specularColor,1.);
	if (hasSpecularMapV == 1)
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
//	fColor = lambertian * diffuse;
//	fColor = vec4((normal*.5)+.5,1.);
}

vec3 GetNormal()
{
    vec3 n;
	if (hasNormalMapV == 1)
	{
		n = 2 * texture(normSampler,vTextureCoord).xyz - 1.;
		if (flipRG){
			float temp = n.y;
			n.y = n.x;
			n.x = temp;
		}
	}
	else n = 2*vec3(.5,.5,1.) -1;

	return normalize(v_TBN * n);
}

vec4 GetDiffuseColor()
{
	vec4 textureColor;
	if (hasDiffuseMapV == 1)
		textureColor = texture(diffSampler, vTextureCoord);
    else
		textureColor = vec4(diffuseTint,1.);

	return textureColor;
}
