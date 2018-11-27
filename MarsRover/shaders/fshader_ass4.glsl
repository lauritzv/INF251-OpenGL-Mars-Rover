#version 330

precision mediump float;

in vec2 vTextureCoord;
in vec3 v;
in vec3 lightPos;
in mat3 v_TBN;

uniform sampler2D diffSampler;
uniform sampler2D normSampler;
uniform sampler2D specSampler;

out vec4 fColor;

const float shinyness = 32.;
const float specularStrength = 2.;
const vec4 diffuseTint = vec4(1.,1.,1.,1.);
	
vec3 GetNormal();
vec3 GetNormal2();
vec3 GetNormal3();

void main() {
    
    //Sample the texture into textureColor.
    //Then we mix it with our ambient, diffuse, and specular colors.
	vec4 textureColor = texture(diffSampler, vTextureCoord);
    vec4 ambient = vec4(.1, .1, .1, 1.) * textureColor;
    vec4 diffuse = diffuseTint * textureColor;
    vec4 specularcolor = texture(specSampler, vTextureCoord) * specularStrength;

    //Calculate the normal
    vec3 normal = GetNormal3();
	
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


// I'm so confused by this...
 vec3 GetNormal(){
    vec3 n = texture(normSampler,vTextureCoord).xyz;
    n = normalize(n * 2.0 - 1.0);
    return (v_TBN * n);
 }

 vec3 GetNormal2(){
    vec3 n = normalize(texture(normSampler,vTextureCoord).xyz);
    return v_TBN * n;
}
 vec3 GetNormal3(){
    vec3 n = texture(normSampler,vTextureCoord).xyz;
    return normalize(v_TBN * n);
}
