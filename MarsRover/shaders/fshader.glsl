#version 330

precision mediump float;

in vec2 vTextureCoord;
in vec3 v;
in vec3 N;
in vec3 lightPos;
in vec3 vertexNormal;
uniform int viewMode;

uniform sampler2D diffSampler;
uniform sampler2D normSampler;
uniform sampler2D specSampler;

out vec4 fColor;

const float shinyness = 32.;
const float specularStrength = 2.;
const bool useTangentSpace = false;
const vec4 diffuseTint = vec4(1.,1.,1.,1.);
	
mat3 calculateTBN();
void main() {
    
    //Sample the texture into textureColor.
    //Then we mix it with our ambient, diffuse, and specular colors.
	vec4 textureColor = texture(diffSampler, vTextureCoord);
    vec4 ambient = vec4(.1, .1, .1, 1.) * textureColor;
    vec4 diffuse = diffuseTint * textureColor;
    vec4 specularcolor = texture(specSampler, vTextureCoord) * specularStrength; 
//    vec4 specularcolor = vec4(1.,1.,1.,1.) * specularStrength;

    //Calculate the normal
	vec3 normalTexture = texture(normSampler, vTextureCoord).xyz;
//	normalTexture = normalTexture *.5 + .5;
    vec3 normal = normalize(N * normalTexture);
	
	vec3 lp = lightPos;
    vec3 vp = v;
	
	if (useTangentSpace) //apply the tangent space matrix
	{
		mat3 TBN = calculateTBN(); //causes facetted faces as I havent computed face-normals.
		normal = TBN * normal;

		//Move light direction and vertex direction to tangent space
		lp = TBN * lightPos;
        vp = TBN * v;
	}

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
	switch(viewMode){
		default:
			fColor = ambient + lambertian * diffuse + specular * specularcolor;
			break;
		case 1:
			fColor = lambertian * diffuse;
			break;
		case 2:
			fColor = specular * specularcolor;
			break;
		case 3:
			fColor = vec4(normal, 1.);
			break;
		case 4: fColor = vec4(N,1.);
			break;
		case 5: fColor = vec4(vertexNormal, 1.);
			break;
	}

//    fColor = ambient + lambertian * diffuse + specular * specularcolor;
    
//	fColor = lambertian * diffuse;
//fColor = vec4(normal, 1.);
//fColor = vec4(1.,1.,1.,1.);
}

mat3 calculateTBN()
{
    //Calculate the tangent and bitangent for the fragment.
    //These are used to create the Tangent Space Transformation matrix.
    //The tangent should be pre-calculated and sendt to the GPU, but for 
    //simplicitys sake we do the calculation locally on the GPU.
    // http://docs.cryengine.com/display/SDKDOC4/Tangent+Space+Normal+Mapping
    vec3 Q1 = dFdx(v);
	vec3 Q2 = dFdy(v);
	vec2 st1 = dFdx(vTextureCoord);
	vec2 st2 = dFdy(vTextureCoord);
 
	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
	vec3 B = normalize(-Q1*st2.s + Q2*st1.s);
    
    //The finished TBN Tangent Space Matrix
    return mat3(T, B, N);;
}
