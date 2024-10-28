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
uniform vec3 CameraPos; // Position of the camera in world space

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;

void main()
{
    vec3 norm = normalize(Normal);

    // Calculate light direction relative to the fragment’s position in world space
    // Important because the camera can move around; Make the light fixed
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

    // Combine lighting components
    vec3 emissiveColor = material.Emissive;
    vec3 lighting = ambient + diffuse + specular + emissiveColor;

    // Apply lighting to the texture color
    vec3 texColor = texture(Texture, TexCoords).rgb;
    FragColor = vec4(lighting * texColor, 1.0);
}
