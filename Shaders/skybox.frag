// shader version
#version 330 core

// Fragment color (R, G, B, A)
out vec4 FragColor;

// Texture Coordinates
in vec3 texCoord;

// Skybox
uniform samplerCube skybox;

// Show color flag
uniform bool showColor;

// Color
uniform vec3 skyboxColor;

void main() {
    if (showColor) {
        FragColor = vec4(skyboxColor, 1.0);
    } else {
        FragColor = texture(skybox, texCoord);
    }
}