#version 460

layout (location = 0) in vec3 VertexPosition;
layout (location = 1) in vec3 VertexNormal;
layout (location = 2) in vec2 VertexTexCoords;

out vec3 FragPos;       // Position of the fragment in world space
out vec3 Normal;        // Normal in world space
out vec2 TexCoords;     // Texture coordinates

uniform mat4 Model;          // Add this for world space transformation
uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 MVP;

void main()
{
    // Transform vertex position to world space using the Model matrix
    FragPos = vec3(Model * vec4(VertexPosition, 1.0));
    
    // Normal vector in world space
    Normal = mat3(transpose(inverse(Model))) * VertexNormal;
    TexCoords = VertexTexCoords; // Pass texture coordinates to fragment shader
    
    // Position the vertex in clip space
    gl_Position = MVP * vec4(VertexPosition, 1.0);
}
