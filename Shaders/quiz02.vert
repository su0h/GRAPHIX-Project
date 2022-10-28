#version 330 core // shader version

// Access attributes in different positions
layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 aTex;

// View Matrix
uniform mat4 view;

// Projection Matrix
uniform mat4 projection;

// Model Matrix
uniform mat4 model;

out vec2 texCoord;
// Pass the processed normals to the fragment shader
out vec3 normCoord;
// Pass the position of the vertex to the fragment shader
out vec3 fragPos;

void main() {
	// Apply projection matrix, view matrix, and model matrix
	gl_Position = projection * view * model * vec4(aPos, 1.0);

	texCoord = aTex;

	// Get the Normal Matrix and convert it into a 3x3 matrix
	// And apply the Normal Matrix to the normal data
	normCoord = mat3(transpose(inverse(model))) * vertexNormal;

	// Apply the Model matrix to the vertex as a vector 3
	fragPos = vec3(model * vec4(aPos, 1.0));
}