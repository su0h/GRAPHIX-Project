/**
 * Fragment shader implementation for textured models.
 *
 * Adapted from: 
 * - https://learnopengl.com/Lighting/Multiple-lights
 * - https://learnopengl.com/Lighting/Light-casters
 */
#version 330 core // Shader version

// Struct that contains directional light attributes
struct DirectionalLight {
	vec3 position;
	
	vec3 lightColor;
	vec3 ambientColor;
	float ambientStr;
	float specularStr;
	float specularPhong;
};

// Struct that contains point light attributes
struct PointLight {
    vec3 position;
    
    float linear;
    float quadratic;
	
	vec3 lightColor;
	vec3 ambientColor;
	float ambientStr;
	float specularStr;
	float specularPhong;
};

// Texture unit for model texture
uniform sampler2D tex0;

// Texture unit for normal mapping
uniform sampler2D norm_tex0;

// If the current model has normal mapping or not
uniform bool hasNormalMapping;
// If the current model has texture/s or not
uniform bool hasTexture;
// If the current model must use its color instead
uniform bool showColor;

// TBN matrix for normal mapping
in mat3 TBN;

// Color of the model
uniform vec3 modelColor;

// Texture Coordinates
in vec2 texCoord;
// Normals Coordinates
in vec3 normCoord;
// Fragment Position
in vec3 fragPos;

// Fragment color (R,G,B,A)
out vec4 FragColor;

// Camera Position
uniform vec3 cameraPos;

// Lights to be used
uniform PointLight pointLight;
uniform DirectionalLight directionalLight;

// Function prototypes for respective light types
vec3 computePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 computeDirectLight(DirectionalLight light, vec3 normal, vec3 viewDir);

void main() {
	// Normalize the received normals coordinates
	vec3 normal;
	
	if (hasNormalMapping) {
		normal = texture(norm_tex0, texCoord).rgb;
		normal = normalize(normal * 2.0 - 1.0);
		normal = normalize(TBN * normal);
	} else {
		normal = normalize(normCoord);
	}

	// Get the view direction from the camera to the fragment
	vec3 viewDir = normalize(cameraPos - fragPos);
    
	// Compute for all the lights needed
	vec3 result = computeDirectLight(directionalLight, normal, viewDir);
	result += computePointLight(pointLight, normal, fragPos, viewDir);

	// Apply everything to the fragment
	// If model has textures, apply texture
	if (!showColor && hasTexture) {
        // Get current pixel color
        vec4 pixelColor = texture(tex0, texCoord);

        // Alpha Cutoff
        if (pixelColor.a < 0.1) {
            // Discard every pixel below 0.1 in alpha
            discard;
        }
        
		FragColor = vec4(result, 1.0f) * pixelColor;       
	// If model has no textures OR model color is toggled, show color only
	} else {
		FragColor = vec4(modelColor, 1.0f);
	}
}

// Compute for point light.
vec3 computePointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir) {
	// Get the direction of the light to the fragment
	vec3 surfaceToLightDir = normalize(light.position - fragPos);
	// Apply diffuse
	float diff = max(dot(normal, surfaceToLightDir), 0.0f);
	// Multiply it to the desired light color
	vec3 diffuse = diff * light.lightColor;
	// Get the ambient light
	vec3 ambientCol = light.ambientColor * light.ambientStr;
	// Gety the reflection vector
	vec3 reflectDir = reflect(-surfaceToLightDir, normal);
	// Get the specular light
	float spec = pow(max(dot(reflectDir, viewDir), 0.1f), light.specularPhong);
	vec3 specColor = spec * light.specularStr * light.lightColor;
    // Prepare attenuation
    float distance = length(light.position - fragPos);
	float attenuation = 1.0f / (1.0f + light.linear * distance + light.quadratic * (distance * distance));
    // Apply attentuation
    ambientCol *= attenuation;
    diffuse *= attenuation;
    specColor *= attenuation;

	return (ambientCol + diffuse + specColor);
}

// Compute for directional light.
vec3 computeDirectLight(DirectionalLight light, vec3 normal, vec3 viewDir) {
	// Get the direction of the light to the fragment
	vec3 surfaceToLightDir = normalize(light.position);
	// Apply diffuse
	float diff = max(dot(normal, surfaceToLightDir), 0.0f);
	// Multiply it to the desired light color
	vec3 diffuse = diff * light.lightColor;
	// Get the ambient light
	vec3 ambientCol = light.ambientColor * light.ambientStr;
	// Gety the reflection vector
	vec3 reflectDir = reflect(-surfaceToLightDir, normal);
	// Get the specular light
	float spec = pow(max(dot(reflectDir, viewDir), 0.1f), light.specularPhong);
	vec3 specColor = spec * light.specularStr * light.lightColor;

	return (ambientCol + diffuse + specColor);
}