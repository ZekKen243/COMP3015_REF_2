#version 460

layout (location = 0) in vec3 VertexPosition;

out vec3 TexCoords;

uniform mat4 MVP;

void main()
{
    TexCoords = VertexPosition;
    gl_Position = MVP * vec4(VertexPosition, 1.0);
    gl_Position = gl_Position.xyww; // Push skybox to background
}
