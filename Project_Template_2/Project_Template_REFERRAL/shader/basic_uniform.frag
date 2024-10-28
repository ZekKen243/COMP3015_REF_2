#version 460

struct Light {
    vec3 Position;      // World space position of the light
    vec3 Ld;            // Diffuse color intensity
    vec3 La;            // Ambient color intensity
    vec3 Ls;            // Specular color intensity
};

struct Material {
    vec3 Kd;            // Diffuse reflectivity
    vec3 Ka;            // Ambient reflectivity
    vec3 Ks;            // Specular reflectivity
    float Shininess;    // Shininess factor for specular highlight size
};

layout (location = 0) out vec4 FragColor;

uniform Light sun;                  // Sun light struct
uniform Material material;           // Material properties struct
uniform sampler2D Texture;           // Texture for the object (e.g., sun.png)

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

void main()
{
    // Normalize the normal vector
    vec3 norm = normalize(Normal);

    // Calculate the light direction (from fragment to light source)
    vec3 lightDir = normalize(sun.Position - FragPos);

    // Diffuse lighting (Lambertian reflection)
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = sun.Ld * material.Kd * diff;

    // Ambient lighting
    vec3 ambient = sun.La * material.Ka;

    // Specular lighting (Blinn-Phong reflection model)
    vec3 viewDir = normalize(-FragPos); // Assuming the camera is at the origin
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), material.Shininess);
    vec3 specular = sun.Ls * material.Ks * spec;

    // Combine all lighting components
    vec3 lighting = ambient + diffuse + specular;

    // Sample the texture color
    vec3 texColor = texture(Texture, TexCoords).rgb;

    // Final color output
    FragColor = vec4(lighting * texColor, 1.0);
}
