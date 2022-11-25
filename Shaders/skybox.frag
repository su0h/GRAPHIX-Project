// shader version
#version 330 core

// Fragment color (R, G, B, A)
out vec4 FragColor;

// Texture Coordinates
in vec3 texCoord;

// Skybox
uniform samplerCube skybox;

void main() {
    FragColor = texture(skybox, texCoord);
}