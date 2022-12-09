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
	// If the skybox uses textures, use the texture
    if (!showColor) {
        FragColor = texture(skybox, texCoord);
	// If the skybox color is toggled, show color only
    } else {
        FragColor = vec4(skyboxColor, 1.0);
    }
}