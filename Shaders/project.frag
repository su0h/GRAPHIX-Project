// Implementation: POINT LIGHT
#version 330 core // shader version
uniform sampler2D tex0;

// Texture Coordinates
in vec2 texCoord;
// Normals Coordinates
in vec3 normCoord;
// Fragment Position
in vec3 fragPos;

// Fragment color (R,G,B,A)
out vec4 FragColor;

// Color input
// uniform vec3 modelColor;

// Light Position
uniform vec3 lightPos;
// Light Color
uniform vec3 lightColor;

// Ambient Light Strength
uniform float ambientStr;
// Amhbient Light Color
uniform vec3 ambientColor;

// Camera Position
uniform vec3 cameraPos;
// Specular Strength
uniform float specStr;
// Specular Phong
uniform float specPhong;

// Point Light linear value
uniform float pointLightLinear;
// Point Light quadratic value
uniform float pointLightQuadratic;

void main() {
	// Normalize the received normals coordinates
	vec3 normal = normalize(normCoord);
	// Get the direction of the light to the fragment
	vec3 lightDir = normalize(lightPos - fragPos);

	// Apply diffuse
	float diff = max(dot(normal, lightDir), 0.0f);
	// Multiply it to the desired light color
	vec3 diffuse = diff * lightColor;

	// Get the ambient light
	vec3 ambientCol = ambientColor * ambientStr;

	// Get the view direction from the camera to the fragment
	vec3 viewDir = normalize(cameraPos - fragPos);
	// Gety the reflection vector
	vec3 reflectDir = reflect(-lightDir, normal);
	// Get the specular light
	float spec = pow(max(dot(reflectDir, viewDir), 0.1f), specPhong);
	vec3 specColor = spec * specStr * lightColor;

    // Prepare attenuation
    float distance = length(lightPos - fragPos);
	float attenuation = 1.0f / (1.0f + pointLightLinear * distance + pointLightQuadratic * (distance * distance));

    // Apply attentuation
    ambientCol *= attenuation;
    diffuse *= attenuation;
    specColor *= attenuation;
    
	// Apply everything to the texture
    FragColor = vec4(specColor + diffuse + ambientCol, 1.0f) * texture(tex0, texCoord);       
}