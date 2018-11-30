#version 330

precision mediump float;

in vec2 vTextureCoord;
in vec3 v;
//in vec3 lightPos;
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
uniform int viewMode;
const float shinyness = 75.;
const float ambientStrength = .1;
	
vec3 GetNormal();
vec4 GetDiffuseColor();
//vec2 newTexCoord;
bool flipRG = true;

// Lights - mostly based on: 
// https://en.wikibooks.org/wiki/GLSL_Programming/GLUT/Multiple_Lights
// after moving away from single light source

struct Light
{
	vec4 position;
	vec4 diffuse;
	vec4 specular;
	float constantAttenuation, linearAttenuation, quadraticAttenuation;
	float spotCutoff, spotExponent;
	vec3 spotDirection;
};

const int numberOfLights = 4;
Light lights[numberOfLights];

// Spotlight 1 (glossy pool)
Light light0 = Light(
	vec4(0.0, 15.0, 10.0, 1.0),
	vec4(2.0,  2.0,  2.0, 1.0),
	vec4(1.0,  1.0,  1.0, 1.0),
	0.01, 0.01, 0.001,
	50.0, 15.0,
	normalize(vec3(0.,-2., 25.) - vec3(0.0,15.0,10.0)));

// Spotlight 2 (Rover headlight)
uniform vec3 headlightPos;
uniform vec3 headlightTarg;
Light light1 = Light(
	vec4(headlightPos, 1.0),
	vec4(1.0,  1.0,  1.0, 1.0),
	vec4(2.0,  2.0,  2.0, 1.0),
	.0, .0001, 0.001,
	40.0, 30.0,
	normalize(headlightTarg - headlightPos));  // norm(target - pos)

// Directional (slightly red)
Light light2 = Light(
	vec4(0.0, 15.0, 10.0, 0.0),
	vec4(0.1,  0.05,  0.05, 0.0),
	vec4(0.2,  0.1,  0.1, 0.0),
	.0, .0, .0,
	0.0, 0.0,
	normalize(vec3(0.,-1.,0.) - vec3(0.0,15.0,10.0)));  // norm(target - pos)

  // Point light
  Light light3 = Light(
	vec4(0.0, -15.0, -55.0, 1.0),
	vec4(1.0,  3.0,  1.0, 1.0),
	vec4(1.0,  1.0,  1.0, 1.0),
	0.01, 0.1, 0.005,
	180.0, 15.0,
	vec3(0.,0.,0.));

void main() 
{
	// set pos and targ dir of rover headlight
	light1.position = vec4(headlightPos,1.);
	light1.spotDirection = normalize(headlightTarg - headlightPos);

	vec4 diffuse = GetDiffuseColor();
	vec4 ambient = ambientStrength * diffuse;

	vec3 totalLighting = ambient.xyz;

    vec4 specularcolor = vec4(specularColor,1.);
	if (hasSpecularMapV == 1)
		specularcolor *= texture(specSampler, vTextureCoord);
    
	//Calculate the normal
    vec3 normal = GetNormal();
	
	lights[0] = light0;
	lights[1] = light1;
	lights[2] = light2;
	lights[3] = light3;
    vec3 vp = v;

    vec3 viewDirection = normalize(-v); // (E) 

	  for (int index = 0; index < numberOfLights; index++) // for all light sources
    {
		float attenuation;
		vec3 lightDirection;
      if (0.0 == lights[index].position.w) // directional light?
		{
		  attenuation = 1.0; // no attenuation
		  lightDirection = normalize(vec3(lights[index].position));
		} 
      else // point light or spotlight (or other kind of light) 
	{
	  vec3 positionToLight =  lights[index].position.xyz - v;
	  float distance = length(positionToLight);
	  lightDirection = normalize(positionToLight);
	  attenuation = 1.0 / (lights[index].constantAttenuation
			       + lights[index].linearAttenuation * distance
			       + lights[index].quadraticAttenuation * distance * distance);
	  
	  if (lights[index].spotCutoff <= 90.0) // spotlight?
		{
			float clampedCosine = max(0.0, dot(-lightDirection, normalize(lights[index].spotDirection)));

			if (clampedCosine < cos(radians(lights[index].spotCutoff))) // outside of spotlight cone?
				attenuation = 0.0;
			else
				attenuation = attenuation * pow(clampedCosine, lights[index].spotExponent);   
		}
	}
      
    vec3 diffuseReflection = attenuation 
	* vec3(lights[index].diffuse) * vec3(diffuse)
	* max(0.0, dot(normal, lightDirection));
      
    vec3 specularReflection;
    if (dot(normal, lightDirection) <= 0.0) // light source on the wrong side?
	{
	specularReflection = vec3(0.0, 0.0, 0.0); // no specular reflection
	}
    else // light source on the right side
	{
	  specularReflection = attenuation * vec3(lights[index].specular) * specularColor.xyz 
	    * pow(max(0.0, dot(reflect(-lightDirection, normal), viewDirection)), shinyness);
	}
      totalLighting += diffuseReflection + specularReflection;
    }


	// last minute addition - reintroducing a few "channel"-views from the previous assignments:
		switch(viewMode){
			default: fColor = vec4(totalLighting,1.); break;
			case 1: fColor = vec4(normal * .5 + .5, 1.); break;
			case 2: fColor = diffuse; break;
			case 3: fColor = ambient * diffuse; break;
		}
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

	


// Move light direction and vertex direction to tangent space
//	lp = v_TBN * lightPos;
//    vp = v_TBN * v;

    //Light direction and vertex directions.
//    vec3 L = normalize(lp - vp);
//	vec3 lightDirection = L;
//	vec3 lightDirection;

    //****BLINN PHONG****//
//    float lambertian = max(dot(L,normal), 0.0);
//    float specular = 0.;
//
//    if(lambertian > 0.) {
//        vec3 halfDir = normalize(L + E);
//        float specAngle = max(dot(halfDir, normal), 0.);
//        specular = pow(specAngle, shinyness);
//    }
    
    //Output final color
//	fColor = ambient + lambertian * diffuse + specular * specularcolor;
//	fColor = lambertian * diffuse;
//	fColor = vec4((normal*.5)+.5,1.);