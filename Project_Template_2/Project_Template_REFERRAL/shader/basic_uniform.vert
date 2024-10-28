#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoords; // UV coordinates

out vec2 TexCoords;    // Pass to fragment shader
out vec3 FragPos;      // World space position for lighting
out vec3 Normal;       // Normal for lighting

uniform mat4 ModelViewMatrix;
uniform mat4 MVP;

void main()
{
    FragPos = vec3(ModelViewMatrix * vec4(VertexPosition, 1.0));
    Normal = mat3(transpose(inverse(ModelViewMatrix))) * VertexNormal;
    TexCoords = VertexTexCoords; // Pass UV coordinates to fragment shader
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}
