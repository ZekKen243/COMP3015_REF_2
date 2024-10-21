#version 460

layout (location = 0) in vec3 VertexPosition;


out vec3 Vec;

uniform mat4 ModelViewMatrix;
uniform mat3 NormalMatrix;
uniform mat4 ProjectionMatrix;
uniform mat4 MVP;

void main()
{
    Vec = VertexPosition;
    gl_Position = MVP * vec4(VertexPosition,1.0);
}
