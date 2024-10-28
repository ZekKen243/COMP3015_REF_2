#version 460

struct Light {
    vec3 Position;
    vec3 Ld;
    vec3 La;
    vec3 Ls;
};

struct Material {
    vec3 Kd;
    vec3 Ka;
    vec3 Ks;
    float Shininess;
    vec3 Emissive;
};

uniform Light light;
uniform Material material;
uniform sampler2D Texture;

// Allows cloud overlay and blending
uniform sampler2D CloudTexture;    
uniform int useClouds;       

// Camera stuff
uniform vec3 CameraPos;            
uniform float time;     

// Roughness and Metallic
uniform float roughness;           
uniform float metallic;            

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    // Keeps the light on the world space; doesn't move with the camera
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.Position - FragPos);
    vec3 viewDir = normalize(CameraPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.Ld * material.Kd * diff * (1.0 - metallic * 0.7);

    vec3 ambient = light.La * (material.Ka + metallic * 0.2);

    vec3 halfwayDir = normalize(lightDir + viewDir);
    float shininessFactor = mix(2.0, 128.0, 1.0 - roughness);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininessFactor);

    // Fresnel 
    float fresnelFactor = 0.1;
    float fresnelIntensity = 2.0;
    float fresnel = fresnelFactor + (1.0 - fresnelFactor) * pow(1.0 - dot(viewDir, norm), 5.0);

    vec3 specular = light.Ls * material.Ks * spec * mix(1.0, fresnelIntensity, fresnel * metallic);

    vec3 emissiveColor = material.Emissive;
    vec3 lighting = ambient + diffuse + specular + emissiveColor;

    vec3 baseColor = texture(Texture, TexCoords).rgb;

    if (useClouds == 1) {
        vec2 cloudCoords = TexCoords + vec2(time * 0.05, 0.0); 
        vec3 cloudColor = texture(CloudTexture, cloudCoords).rgb;
        baseColor = mix(baseColor, cloudColor, 0.4); 
    }

    FragColor = vec4(lighting * baseColor, 1.0);
}
