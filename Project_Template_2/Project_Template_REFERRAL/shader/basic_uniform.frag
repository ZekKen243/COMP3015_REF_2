#version 460

struct Light {
    vec3 Position;      // World-space position of the light
    vec3 Ld;            // Diffuse 
    vec3 La;            // Ambient 
    vec3 Ls;            // Specular 
};

struct Material {
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ka;            // Ambient reflectivity
    vec3 Ks;            // Specular reflectivity
    float Shininess;    // Shininess factor
    vec3 Emissive;      // Emissive "glow"
};

uniform Light light;
uniform Material material;
uniform sampler2D Texture;         
uniform sampler2D CloudTexture;    
uniform int useClouds;             // 1 for planet, 0 for others
uniform vec3 CameraPos;            
uniform float time;    

uniform float roughness;    
uniform float metallic;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.Position - FragPos);    // Light direction
    vec3 viewDir = normalize(CameraPos - FragPos);          // View direction from camera
    vec3 halfwayDir = normalize(lightDir + viewDir);        // Blinn-Phong halfway vector

    // Diffuse (Lambertian)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.Ld * material.Kd * diff * (1.0 - metallic * 0.7);

    // Specular with roughness (adjust Shininess based on roughness)
    float shininessFactor = mix(32.0, 2.0, roughness);   // Higher roughness softens highlights
    float spec = pow(max(dot(norm, halfwayDir), 0.0), shininessFactor);
    vec3 specular = light.Ls * material.Ks * spec * (0.5 + metallic * 0.5);

    // Ambient lighting calculation
    vec3 ambient = light.La * (material.Ka + metallic * 0.2);

    // Emissive component (for self-illuminated objects like the sun)
    vec3 emissiveColor = material.Emissive;

    // Mix metallic: blend between diffuse+specular and only specular for metal-like look
    vec3 lighting = ambient + mix(diffuse + specular, specular, metallic) + emissiveColor;

    // Base planet texture color
    vec3 baseColor = texture(Texture, TexCoords).rgb;

    // Apply cloud overlay only if useClouds is set
    if (useClouds == 1) {
        vec2 cloudCoords = TexCoords + vec2(time * 0.05, 0.0); // Horizontal scrolling
        vec3 cloudColor = texture(CloudTexture, cloudCoords).rgb;
        baseColor = mix(baseColor, cloudColor, 0.4);           // Blend planet and clouds
    }

    // Final color with lighting applied
    FragColor = vec4(lighting * baseColor, 1.0);
}
