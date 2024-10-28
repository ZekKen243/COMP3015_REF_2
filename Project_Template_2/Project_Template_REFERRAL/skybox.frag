#version 460

in vec3 TexCoords;
layout (binding = 0) uniform samplerCube SkyBoxTex;
out vec4 FragColor;

void main()
{
    vec3 texColor = texture(SkyBoxTex, TexCoords).rgb;
    FragColor = vec4(texColor, 1.0);
}
