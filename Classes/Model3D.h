#pragma once

// 3D Model class implementation. Handles the parameters and displaying of a single model.
class Model3D {
private:
	// Vertex data of the 3D model
	std::vector<GLfloat> fullVertexData;
	// Position (X, Y, Z) of the 3D model
	glm::vec3 position;
	// Theta Rotation (X, Y, Z) of the 3D model
	glm::vec3 rotation;
	// Scale (X, Y, Z) of the 3D model
	glm::vec3 scale;
	// Color (in RGB) of the 3D model
	glm::vec3 color;
	// Texture of the 3D model
	GLuint texture;
	// VAO of the 3D model
	GLuint VAO;
	// VBO of the 3D model
	GLuint VBO;
	// If the 3D model has normals or not
	bool hasNormals;
	// If the 3D model has texture coordinates or not
	bool hasTexcoords;

	// Loads the indices (vertices, normals, & UV) of a 3D model.
	void loadIndexData(const char* modelPath) {
		std::cout << "----------------------------------------------------------------" << std::endl;
		std::cout << "MODEL INITIALIZATION" << std::endl;
		std::cout << "----------------------------------------------------------------" << std::endl;
		std::cout << "Loading model indices from: \"" << modelPath << "\"" << std::endl;

		// Preparation for loading the 3D model
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warning, error;
		tinyobj::attrib_t attributes;

		// Load the model itself
		bool isModelLoaded = tinyobj::LoadObj(
			&attributes,
			&shapes,
			&materials,
			&warning,
			&error,
			modelPath
		);

		// Check if the model has been loaded successfully
		if (isModelLoaded) {

			// Check if the 3D model has normals
			if (attributes.normals.size() <= 0) {
				this->hasNormals = false;
				std::cout << "WARNING: No normals data was found." << std::endl;
			}

			// Check if the 3D model has texture coordinates
			if (attributes.texcoords.size() <= 0) {
				this->hasTexcoords = false;
				std::cout << "WARNING: No texcoords data was found." << std::endl;
			}

			// Iterate through the mesh indices of the model and extract all the vertex data
			for (int i = 0; i < shapes.size(); i++) {
				for (int j = 0; j < shapes[i].mesh.indices.size(); j++) {
					// Load mesh data
					tinyobj::index_t vData = shapes[i].mesh.indices[j];

					// Load vertex index
					int vertexIndex = vData.vertex_index * 3;

					// Vertex X 
					this->fullVertexData.push_back(attributes.vertices[vertexIndex]);

					// Vertex Y 
					this->fullVertexData.push_back(attributes.vertices[(vertexIndex + 1)]);

					// Vertex Z 
					this->fullVertexData.push_back(attributes.vertices[(vertexIndex + 2)]);

					// If the 3D Model has normals
					if (hasNormals) {
						// Load normals index
						int normalsIndex = vData.normal_index * 3;

						// Normals X
						this->fullVertexData.push_back(attributes.normals[normalsIndex]);

						// Normals Y
						this->fullVertexData.push_back(attributes.normals[(normalsIndex + 1)]);

						// Normals Z
						this->fullVertexData.push_back(attributes.normals[(normalsIndex + 2)]);
					}

					// If the 3D Model has texture coordinates
					if (hasTexcoords) {
						// Load texture coordinate indices
						int uvIndex = vData.texcoord_index * 2;

						// U 
						this->fullVertexData.push_back(attributes.texcoords[uvIndex]);

						// V 
						this->fullVertexData.push_back(attributes.texcoords[(uvIndex + 1)]);
					}
				}
			}

			std::cout << "Model loaded successfully!" << std::endl;
		}
		// Else, show warning/s and error/s
		else {
			std::cout << "ERROR: Unable to load \"" << modelPath << "\"" << std::endl;
		}

	}

	// Bind the vertex data of the 3D model to a VAO and VBO.
	void bindVertexData() {
		// Adapt stride and pointer offset for glVertexAttribPointer 
		// depending on if 3D Model has normals and/or texture coordinates
		int vDataLen = hasNormals && hasTexcoords ? 8 : hasNormals ? 6 : hasTexcoords? 5 : 3;
		int ptrOffset = hasNormals ? 3 : 0;

		// Generate VAO
		glGenVertexArrays(1, &this->VAO);
		// Generate VBO
		glGenBuffers(1, &this->VBO);

		// Bind the 3D model's data onto the created VBO
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glBufferData(
			GL_ARRAY_BUFFER,
			sizeof(GL_FLOAT) * this->fullVertexData.size(),
			this->fullVertexData.data(),
			GL_STATIC_DRAW
		);

		// Bind the 3D model's data onto the VAO
		glBindVertexArray(this->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
		glVertexAttribPointer(
			0,
			3,
			GL_FLOAT,
			GL_FALSE,
			vDataLen * sizeof(GL_FLOAT),
			(void*)0
		);

		// Enable index 0 (vertex positions)
		glEnableVertexAttribArray(0);

		// If the 3D Model has normals
		if (hasNormals) {
			// Tell OpenGL to use the normals points
			GLintptr normPtr = ptrOffset * sizeof(GLfloat);
			glVertexAttribPointer(
				1,
				3,
				GL_FLOAT,
				GL_FALSE,
				vDataLen * sizeof(GL_FLOAT),
				(void*)normPtr
			);

			// Enable index 1 (normals)
			glEnableVertexAttribArray(1);
		}

		// If the 3D Model has texture coordinates
		if (hasTexcoords) {
			// Tell OpenGL to use the latter 2 data points as U and V
			GLintptr uvPtr = (ptrOffset + 3) * sizeof(GLfloat);
			glVertexAttribPointer(
				2,
				2,
				GL_FLOAT,
				GL_FALSE,
				vDataLen * sizeof(GL_FLOAT),
				(void*)uvPtr
			);

			// Enable index 2 (textures)
			glEnableVertexAttribArray(2);
		}

		// Reset buffer bindings
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	// Bind the texture of the 3D model.
	void bindTexture(const char* texturePath) {
		// If given texture path is not NULL, proceed as usual
		if (texturePath != NULL) {
			std::cout << "Loading texture from: \"" << texturePath << "\"" << std::endl;

			// Load textures
			stbi_set_flip_vertically_on_load(true);
			int img_width, img_height, color_channels;
			unsigned char* texture_bytes = stbi_load(
				texturePath,
				&img_width,
				&img_height,
				&color_channels,
				0
			);

			// If texture was successfully loaded
			if (texture_bytes != NULL) {
				// Generate texture
				glGenTextures(1, &this->texture);
				// Activate texture
				glActiveTexture(GL_TEXTURE0);
				// Bind texture
				glBindTexture(GL_TEXTURE_2D, this->texture);

				// Define the texture
				// If RGBA image (i.e., PNG)
				if (color_channels > 3)
					glTexImage2D(
						GL_TEXTURE_2D,
						0,
						GL_RGB,
						img_width,
						img_height,
						0,
						GL_RGBA,
						GL_UNSIGNED_BYTE,
						texture_bytes
					);
				// If RGB image (i.e., JPG)
				else
					glTexImage2D(
						GL_TEXTURE_2D,
						0,
						GL_RGB,
						img_width,
						img_height,
						0,
						GL_RGB,
						GL_UNSIGNED_BYTE,
						texture_bytes
					);

				// Generate texture
				glGenerateMipmap(GL_TEXTURE_2D);

				// Since done already, clear memory (to save space)
				stbi_image_free(texture_bytes);

				std::cout << "Texture loaded successfully!" << std::endl;
			}
			// Else, show error for failure to load texture
			else {
				std::cout << "ERROR: Unable to load \"" << texturePath << "\"" << std::endl;
			}
		}
		// Else, show error for NULL path
		else {
			std::cout << "ERROR: Given texture path is NULL. "<< std::endl;
		}

		std::cout << "----------------------------------------------------------------" << std::endl << std::endl;
	}

public:
	// Instantiates the 3D model with default values for position, rotation, scale, and color.
	Model3D(
		const char* objPath,
		const char* texturePath
	) {
		// Initialize attributes to its default values
		this->position = glm::vec3(0.0f, 0.0f, 0.0f);
		this->rotation = glm::vec3(0.0f, 0.0f, 0.0f);
		this->scale = glm::vec3(1.0f, 1.0f, 1.0f);
		this->color = glm::vec3(1.0f, 1.0f, 1.0f);
		this->hasNormals = true;
		this->hasTexcoords = true;

		// Proceed to loading and binding the respective model data
		this->loadIndexData(objPath);
		this->bindTexture(texturePath);
		this->bindVertexData();
	}

	// Instantiates the 3D model with specified values for position, rotation, and scale.
	Model3D(
		const char* objPath,
		const char* texturePath,
		glm::vec3 position,
		glm::vec3 rotation,
		glm::vec3 scale
	) {
		// Initialize attributes based on parameters given
		this->position = position;
		this->rotation = rotation;
		this->scale = scale;

		// Initialize some sttributes to its default values
		this->color = glm::vec3(1.0f, 1.0f, 1.0f);
		this->hasNormals = true;
		this->hasTexcoords = true;

		// Proceed to loading and binding the respective model data
		this->loadIndexData(objPath);
		this->bindTexture(texturePath);
		this->bindVertexData();
	}

	// Instantiates the 3D model with specified values for position, rotation, scale, and color.
	Model3D(
		const char* objPath,
		const char* texturePath,
		glm::vec3 position,
		glm::vec3 rotation,
		glm::vec3 scale, 
		glm::vec3 color
	) {
		// Initialize attributes based on parameters given
		this->position = position;
		this->rotation = rotation;
		this->scale = scale;
		this->color = color;

		// Initialize some sttributes to its default values
		this->hasNormals = true;
		this->hasTexcoords = true;

		// Proceed to loading and binding the respective model data
		this->loadIndexData(objPath);
		this->bindTexture(texturePath);
		this->bindVertexData();
	}

	// Draw the model using the shader.
	void draw(Shader modelShader) {
		// Bind the model's VAO
		glBindVertexArray(this->VAO);

		// Initialize the Model Matrix as an identity matrix
		glm::mat4 transformationMatrix = glm::mat4(1.0f);

		// Apply a translation to the Model Matrix
		transformationMatrix = glm::translate(
			transformationMatrix,
			this->position
		);

		// Apply scaling to the Model Matrix
		transformationMatrix = glm::scale(
			transformationMatrix,
			this->scale
		);

		// Apply an X-axis rotation to the Model Matrix
		transformationMatrix = glm::rotate(
			transformationMatrix,
			glm::radians((float)this->rotation.x),
			glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f))
		);

		// Apply a Y-axis rotation to the Model Matrix
		transformationMatrix = glm::rotate(
			transformationMatrix,
			glm::radians((float)this->rotation.y),
			glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))
		);

		// Apply a Z-axis rotation to the Model Matrix
		transformationMatrix = glm::rotate(
			transformationMatrix,
			glm::radians((float)this->rotation.z),
			glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f))
		);

		// Link the Model Matrix to the shader program
		modelShader.setMat4("model", transformationMatrix);

		// Bind the 3D model's texture
		glBindTexture(GL_TEXTURE_2D, this->texture);
		modelShader.setInt("tex0", 0);

		// Bind the 3D model's color
		modelShader.setVec3("modelColor", this->color);

		// Draw the model itself
		glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 5);
		glBindVertexArray(0);
	}

	// Set the position of the 3D model.
	void setPosition(glm::vec3 newPosition) {
		this->position = newPosition;
	}

	// Set the rotation of the 3D model.
	void setRotation(glm::vec3 newRotation) {
		this->rotation = newRotation;
	}

	// Set the scaling of the 3D model.
	void setScale(glm::vec3 newScale) {
		this->scale = newScale;
	}

	// Retrieve the position of the 3D model.
	glm::vec3 getPosition() {
		return glm::vec3(position.x, position.y, position.z);
	}

	// Retrieve the rotation of the 3D model.
	glm::vec3 getRotation() {
		return glm::vec3(rotation.x, rotation.y, rotation.z);
	}

	// Retrieve the scaling of the 3D model.
	glm::vec3 getScale() {
		return glm::vec3(scale.x, scale.y, scale.z);
	}
};