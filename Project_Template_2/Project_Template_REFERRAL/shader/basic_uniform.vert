#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;

out vec3 Position;
out vec3 Normal;

out vec3 LightIntensity;
// flat out vec3 LightIntensity;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
    //vec3 n = normalize(NormalMatrix * VertexNormal);
    //vec4 pos = ModelViewMatrix * vec4(VertexPosition, 1.0);
    Normal = normalize(NormalMatrix * VertexNormal);
    Position = (ModelViewMatrix * vec4(VertexPosition, 1.0)).xyz;
   
    gl_Position = MVP * vec4(VertexPosition,1.0);
}
