/**
 * Vertex shader implementation for textured models.
 * 
 * Adapted from: 
 * - https://learnopengl.com/Lighting/Multiple-lights
 * - https://learnopengl.com/Lighting/Light-casters
 */
#version 330 core // Shader version

// Access attributes in different positions
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 aTex;
layout(location = 3) in vec3 m_tan;
layout(location = 4) in vec3 m_btan;

// View Matrix
uniform mat4 view;

// Projection Matrix
uniform mat4 projection;

// Model Matrix
uniform mat4 model;

// Pass the coordinates of the textures to the fragment shader
out vec2 texCoord;
// Pass the processed normals to the fragment shader
out vec3 normCoord;
// Pass the position of the vertex to the fragment shader
out vec3 fragPos;

// TBN matrix for normal mapping
out mat3 TBN;

void main() {
	// Apply projection matrix, view matrix, and model matrix
	gl_Position = projection * view * model * vec4(aPos, 1.0);

	// Retrieve texture coordinates
	texCoord = aTex;

	// Get the Normal Matrix and convert it into a 3x3 matrix
	// And apply the Normal Matrix to the normal data
	mat3 modelMat = mat3(transpose(inverse(model)));
	normCoord = modelMat * vertexNormal;

	// Compute for TBN matrix
	vec3 T = normalize(modelMat * m_tan);
	vec3 B = normalize(modelMat * m_btan);
	vec3 N = normalize(normCoord);
	TBN = mat3(T, B, N);

	// Apply the Model matrix to the vertex as a vector 3
	fragPos = vec3(model * vec4(aPos, 1.0));
}