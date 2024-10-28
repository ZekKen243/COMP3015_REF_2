#version 460
in vec2 TexCoords;
uniform sampler2D scene;
out vec4 FragColor;

void main() {
    vec2 tex_offset = 1.0 / textureSize(scene, 0); // Texture coordinate offset
    vec3 result = vec3(0.0);

    float weights[5] = float[](0.227027, 0.1945946, 0.1216216, 0.054054, 0.016216);
    for (int i = -4; i <= 4; i++) {
        result += texture(scene, TexCoords + vec2(0.0, tex_offset.y * i)).rgb * weights[abs(i)];
    }
    FragColor = vec4(result, 1.0);
}
