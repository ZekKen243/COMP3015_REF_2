#version 460

in vec3 Position;
in vec3 Normal;
in vec2 TexCoord;
// flat in vec3 LightIntensity;

layout (binding = 0) uniform sampler2D brickTex;
layout (binding = 1) uniform sampler2D mossTex;
layout (location = 0) out vec4 FragColor;

//uniform struct LightInfo
//{
//    vec4 Position;
//    vec3 La;
//    vec3 L; 
//} lights [3];

/*uniform struct SpotLightInfo
{
    vec3 Position;
    vec3 La;
    vec3 L; 
    vec3 Direction;
    float Exponent;
    float Cutoff;
} Spot;*/

uniform struct LightInfo
{
    vec4 Position;
    vec3 La;
    vec3 L; 
} Light;

uniform struct MaterialInfo
{
    vec3 Kd;
    vec3 Ka;
    vec3 Ks;
    float Shininess;
}Material;

/*uniform struct FogInfo
{
    float MaxDist;
    float MinDist;
    vec3 Color;
}Fog;*/

// For toon shading
/*const int levels = 3;
const float scaleFactor = 1.0 / levels;*/

//vec3 phongModel( int light, vec3 position, vec3 n)
//{
    // Ambient calculation
//    vec3 ambient = lights[light].La * Material.Ka;

    // Diffuse calculation
//    vec3 s = normalize(vec3(lights[light].Position.xyz) - position);
//    float sDotN = max(dot(s,n), 0.0);
//    vec3 diffuse = Material.Kd * sDotN;

    // Specular calculation
//    vec3 spec = vec3(0.0);

//    if (sDotN > 0.0) 
//    {
//        vec3 v = normalize(-position.xyz);
//        vec3 r = reflect(-s, n);
//        spec = Material.Ks * pow(max(dot(r, v), 0.0), Material.Shininess);
//    }

//    return ambient + (diffuse + spec) * lights[light].L;
//}

/*vec3 blinnphongSpot(vec3 position, vec3 n)
{
    vec3 diffuse = vec3(0), spec = vec3(0);
    // Ambient calculation
    vec3 ambient = Spot.La * Material.Ka;

    // Diffuse calculation
    vec3 s = normalize(Spot.Position - position);

    float cosAng = dot(-s, normalize(Spot.Direction));
    float angle = acos(cosAng);
    float spotScale;

    if (angle >= 0.0 && angle < Spot.Cutoff)
    {
        spotScale = pow(cosAng, Spot.Exponent);
        float sDotN = max(dot(s,n), 0.0);
        //diffuse = Material.Kd * sDotN;
        // For toon shading
        diffuse = Material.Kd * floor(sDotN * levels) * scaleFactor;

        if (sDotN > 0.0) 
        {   
            vec3 v = normalize(-position.xyz);
            vec3 h = normalize(v + s);
            spec = Material.Ks * pow(max(dot(h, n), 0.0), Material.Shininess);
        }
    }

    return ambient + spotScale * (diffuse + spec) * Spot.L;
}*/

vec3 blinnPhong(vec3 position, vec3 n)
{
    vec3 diffuse = vec3(0), spec = vec3(0);

    vec4 brickTexColor = texture(brickTex, TexCoord);
    vec4 mossTexColor = texture(mossTex, TexCoord);
    vec3 texColor = mix(brickTexColor.rgb, mossTexColor.rgb, mossTexColor.a);


    // Ambient calculation
    //vec3 ambient = Light.La * Material.Ka;
    vec3 ambient = Light.La * texColor;

    // Diffuse calculation
    vec3 s = normalize(Light.Position.xyz - position);

    float sDotN = max(dot(s,n), 0.0);
    //diffuse = Material.Kd * sDotN;
    diffuse = texColor * sDotN;


    if (sDotN > 0.0) 
    {   
        vec3 v = normalize(-position.xyz);
        vec3 h = normalize(v + s);
        spec = Material.Ks * pow(max(dot(h, n), 0.0), Material.Shininess);
    }
    
    return ambient + (diffuse + spec) * Light.L;
}

void main() {

    //vec3 Color = vec3(0.0);
    //for (int i = 0; i < 3; i++)
    //    Color += phongModel(i, Position, Normal);

    //FragColor = vec4(Color, 1.0);

    /*float dist = abs(Position.z);
    float fogFactor = (Fog.MaxDist - dist)/(Fog.MaxDist - Fog.MinDist);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    vec3 shadeColor = blinnPhong(Position, normalize(Normal));*/

    //vec3 color = mix(Fog.Color, shadeColor, fogFactor);

    //FragColor = vec4(color, 1.0);
    FragColor = vec4(blinnPhong(Position, normalize(Normal)), 1.0);
}
