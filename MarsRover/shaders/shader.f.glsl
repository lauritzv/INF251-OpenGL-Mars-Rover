#version 330

precision mediump float;

in vec2 vTextureCoord;
in vec3 v;
in vec3 N;
in vec3 lightPos;

//The sampler2D uniforms represent the 2 textures we are going to use.
uniform sampler2D uSampler;
uniform sampler2D uNormalSampler;
out vec4 fColor;


void main() {
    
    //Sample the texture into textureColor.
    //Then we mix it with our ambient, diffuse, and specular colors.
    vec4 textureColor = texture(uSampler, vTextureCoord);
    vec4 ambient = vec4(0.1, 0.1, 0.1, 1.0) * textureColor;
    vec4 diffuse = vec4(0.5, 0.5, 0.5, 1.0) * textureColor;
    vec4 specularcolor = vec4(0.8, 0.8, 0.8, 1.0) * textureColor; 
    
    //Calculate the normal and apply the tangent space matrix
    vec3 normal = texture(uNormalSampler, vTextureCoord).xyz;
    normal = normalize(normal * 2.0 - 1.0);
        //normal = normalize(TBN * normal);
    
    vec3 lp = lightPos;
    vec3 vp = v;
    
	//Light direction and vertex directions.
    vec3 L = normalize(lp - vp);
    vec3 E = normalize(-vp);

    //****BLINN PHONG****//
    float lambertian = max(dot(L,normal), 0.0);
    float specular = 0.0;

    if(lambertian > 0.0) {
        vec3 halfDir = normalize(L + E);
        float specAngle = max(dot(halfDir, normal), 0.0);
        specular = pow(specAngle, 16.0);
    }
    //****BLINN PHONG****//
    
    //Output final color
    fColor = ambient + lambertian * diffuse + specular * specularcolor;
}
