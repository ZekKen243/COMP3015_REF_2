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

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    vec3 norm = normalize(Normal);

    // Calculate light direction relative to the fragment’s position in world space
    vec3 lightDir = normalize(light.Position - FragPos);

    // View direction from the camera to the fragment
    vec3 viewDir = normalize(CameraPos - FragPos);

    // Diffuse lighting calculation
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.Ld * material.Kd * diff;

    // Ambient lighting calculation
    vec3 ambient = light.La * material.Ka;

    // Specular lighting (Blinn-Phong)
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.Shininess);
    vec3 specular = light.Ls * material.Ks * spec;

    // Combine lighting components with emissive color
    vec3 emissiveColor = material.Emissive;
    vec3 lighting = ambient + diffuse + specular + emissiveColor;

    // Base planet texture color
    vec3 baseColor = texture(Texture, TexCoords).rgb;

    // Apply cloud overlay only if useClouds is set
    if (useClouds == 1) {
        // Animated cloud
        vec2 cloudCoords = TexCoords + vec2(time * 0.05, 0.0); 
        vec3 cloudColor = texture(CloudTexture, cloudCoords).rgb;

        baseColor = mix(baseColor, cloudColor, 0.4); 
    }

    FragColor = vec4(lighting * baseColor, 1.0);
}
