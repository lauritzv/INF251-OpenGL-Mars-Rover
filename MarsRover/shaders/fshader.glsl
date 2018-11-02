#version 300 es

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
    
    //We calculate the tangent and bitangent for the fragment.
    //These are used to create the Tangent Space Transformation matrix.
    //The tangent space is usually pre calculated and sendt to the GPU, but for 
    //simplicitys sake we do the calculation locally on the GPU.
    //Read more about this at: http://docs.cryengine.com/display/SDKDOC4/Tangent+Space+Normal+Mapping
//    vec3 Q1 = dFdx(v);
//	vec3 Q2 = dFdy(v);
//	vec2 st1 = dFdx(vTextureCoord);
//	vec2 st2 = dFdy(vTextureCoord);
 
//	vec3 T = normalize(Q1*st2.t - Q2*st1.t);
//	vec3 B = normalize(-Q1*st2.s + Q2*st1.s);
    
    //The finished Tangent Space Matrix
        //mat3 TBN = mat3(T, B, N);

    //Calculate the normal and apply the tangent space matrix
    vec3 normal = texture(uNormalSampler, vTextureCoord).xyz;
    normal = normalize(normal * 2.0 - 1.0);
        //normal = normalize(TBN * normal);
    
    //Move light direction and vertex direction to tangent space
        //vec3 lp = TBN * lightPos;
        //vec3 vp = TBN * v;
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
