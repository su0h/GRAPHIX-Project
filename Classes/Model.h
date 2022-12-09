#pragma once

/*
    3D Model class implementation. Holds every model-related functionality.
 */
class Model {
private:
    // The data of the .obj file provided for this model
    std::vector<GLfloat> fullVertexData;
    // Position of the model
    glm::vec3 position;
    // Rotation value of the model
    glm::vec3 rotation;
    // Scale of the model
    glm::vec3 scale;
    // Color of the model
    glm::vec3 color;

    // Flag to determine if the model has normal coordinates
    bool hasNormals;
    // Flag to determine if the model has texture coordinates
    bool hasTexCoords;
    // Flag to determine if the model uses normal mapping
    bool hasNormalMapping;
    // Flag to determine if the model has textures
    bool hasTexture;
    // Flag for showing the model color or not
    bool showColor;

    // List that contains the textures of this model
    std::vector<Texture> textures;
    // List that contains the normals of this model
    Texture normalMap;

    // The Vertex Array Object of this model
    GLuint VAO;
    // The Vertex Buffer Object of this model
    GLuint VBO;
    // The length of the data of this model
    int dataLen;

    // Limit on the textures to be loaded
    const int TEXT_LIMIT = 1;
    // Offset value for textures and normal maps
    const int TEXT_OFFSET = 9;
    // Size of vertex components (XYZ)
    const int VERT_SIZE = 3;
    // Size of normals components (XYZ)
    const int NORM_SIZE = 3;
    // Size of texture coordinates components (UV)
    const int UV_SIZE = 2;
    // Size of tangent components (XYZ)
    const int TAN_SIZE = 3;
    // Size of bitangent components (XYZ)
    const int BITAN_SIZE = 3;
    // Location of Normal Map
    const int NORM_MAP_LOC = 9;

    // Loads the data inside the .obj file provided. 
    void loadObjData(std::string path) {
        std::cout << "Loading model data from: " << path << std::endl;

        // Will contain the mesh's shapes
        std::vector<tinyobj::shape_t> shapes;
        // Will contain the mesh's materials
        std::vector<tinyobj::material_t> materials;
        // Some error messages might pop up
        std::string warning, error;
        // Basic attributes related to the mesh
        tinyobj::attrib_t attributes;

        // Load the .obj file contents
        bool isModelLoaded = tinyobj::LoadObj(
            &attributes,
            &shapes,
            &materials,
            &warning,
            &error,
            path.c_str()
        );

        // If .obj file was successfully loaded
        if (isModelLoaded) {
            std::cout << "Model loaded successfully! " << std::endl;
            std::cout << "Binding the data... " << std::endl;

            // Check if the 3D model has normals
            if (attributes.normals.size() <= 0) {
                this->hasNormals = false;
                std::cout << "WARNING: No normals data was found." << std::endl;
            }

            // Check if the 3D model has texture coordinates
            if (attributes.texcoords.size() <= 0) {
                this->hasTexCoords = false;
                std::cout << "WARNING: No texcoords data was found." << std::endl;
            }

            // For normal mapping
            std::vector<glm::vec3> tangents;
            std::vector<glm::vec3> bitangents;

            // If the model uses normal mapping
            if (hasNormalMapping) {
                for (int i = 0; i < shapes[0].mesh.indices.size(); i += 3) {
                    // Get data from indices
                    tinyobj::index_t vData1 = shapes[0].mesh.indices[i];
                    tinyobj::index_t vData2 = shapes[0].mesh.indices[i + 1];
                    tinyobj::index_t vData3 = shapes[0].mesh.indices[i + 2];

                    // XYZ from data 1
                    glm::vec3 v1 = glm::vec3(
                        attributes.vertices[vData1.vertex_index * 3],
                        attributes.vertices[vData1.vertex_index * 3 + 1],
                        attributes.vertices[vData1.vertex_index * 3 + 2]
                    );

                    // XYZ from data 2
                    glm::vec3 v2 = glm::vec3(
                        attributes.vertices[vData2.vertex_index * 3],
                        attributes.vertices[vData2.vertex_index * 3 + 1],
                        attributes.vertices[vData2.vertex_index * 3 + 2]
                    );

                    // XYZ from data 3
                    glm::vec3 v3 = glm::vec3(
                        attributes.vertices[vData3.vertex_index * 3],
                        attributes.vertices[vData3.vertex_index * 3 + 1],
                        attributes.vertices[vData3.vertex_index * 3 + 2]
                    );

                    // UV from data 1
                    glm::vec2 uv1 = glm::vec2(
                        attributes.texcoords[vData1.texcoord_index * 2],
                        attributes.texcoords[vData1.texcoord_index * 2 + 1]
                    );

                    // UV from data 2
                    glm::vec2 uv2 = glm::vec2(
                        attributes.texcoords[vData2.texcoord_index * 2],
                        attributes.texcoords[vData2.texcoord_index * 2 + 1]
                    );

                    // UV from data 3
                    glm::vec2 uv3 = glm::vec2(
                        attributes.texcoords[vData3.texcoord_index * 2],
                        attributes.texcoords[vData3.texcoord_index * 2 + 1]
                    );

                    // Compute for tangents and bitangents
                    glm::vec3 deltaPos1 = v2 - v1;
                    glm::vec3 deltaPos2 = v3 - v1;

                    glm::vec2 deltaUV1 = uv2 - uv1;
                    glm::vec2 deltaUV2 = uv3 - uv1;

                    float r = 1.f / ((deltaUV1.x * deltaUV2.y) - (deltaUV1.y * deltaUV2.x));

                    glm::vec3 tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y) * r;
                    glm::vec3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x) * r;

                    tangents.push_back(tangent);
                    tangents.push_back(tangent);
                    tangents.push_back(tangent);

                    bitangents.push_back(bitangent);
                    bitangents.push_back(bitangent);
                    bitangents.push_back(bitangent);
                }
            }

            // Iterate through the indices of the model
            for (int i = 0; i < shapes.size(); i++) {
                for (int j = 0; j < shapes[i].mesh.indices.size(); j++) {
                    tinyobj::index_t vData = shapes[i].mesh.indices[j];

                    // Get offset for XYZ
                    int vertexIndex = vData.vertex_index * 3;

                    // X
                    fullVertexData.push_back(
                        attributes.vertices[vertexIndex]
                    );
                    // Y
                    fullVertexData.push_back(
                        attributes.vertices[vertexIndex + 1]
                    );
                    // Z
                    fullVertexData.push_back(
                        attributes.vertices[vertexIndex + 2]
                    );

                    // If the model has normals
                    if (hasNormals) {
                        // Get offset for normals
                        int normalIndex = vData.normal_index * 3;

                        // Normal index 1
                        fullVertexData.push_back(
                            attributes.normals[normalIndex]
                        );
                        // Normal index 2
                        fullVertexData.push_back(
                            attributes.normals[normalIndex + 1]
                        );
                        // Normal index 3
                        fullVertexData.push_back(
                            attributes.normals[normalIndex + 2]
                        );
                    }

                    // If the model has texture coordinates
                    if (hasTexCoords) {
                        // Get offset for UV
                        int uvIndex = vData.texcoord_index * 2;

                        // U
                        fullVertexData.push_back(
                            attributes.texcoords[uvIndex]
                        );
                        // V
                        fullVertexData.push_back(
                            attributes.texcoords[uvIndex + 1]
                        );
                    }

                    // If the model has normal mapping
                    if (hasNormalMapping) {
                        // Tangents for normal map
                        fullVertexData.push_back(
                            tangents[i].x
                        );
                        fullVertexData.push_back(
                            tangents[i].y
                        );
                        fullVertexData.push_back(
                            tangents[i].z
                        );

                        // Bitangents for normal map
                        fullVertexData.push_back(
                            bitangents[i].x
                        );
                        fullVertexData.push_back(
                            bitangents[i].y
                        );
                        fullVertexData.push_back(
                            bitangents[i].z
                        );
                    }
                }
            }

            std::cout << "Data bound succesfully!" << std::endl;
        }
        else {
            // Show warning and error messages
            std::cout << warning << " | " << error << std::endl;
        }
    }

    // Loads the textures of this model given a list of file paths.
    void loadTextures(std::vector<std::string> paths) {
        // Flip images on load
        stbi_set_flip_vertically_on_load(true);

        // Check if there are more than 8 texture paths
        if (paths.size() > TEXT_LIMIT)
            std::cout << "WARNING: Only upto " << TEXT_LIMIT << " textures will be loaded." << std::endl;

        // Iterate through each texture path specified
        for (int i = 0; i < paths.size() && i < TEXT_LIMIT; i++) {
            const char* path = paths[i].c_str();

            std::cout << "Loading textures from " << path << std::endl;

            // Load texture from current path
            int imgWidth, imgHeight, colorChannels;
            unsigned char* tex_bytes = stbi_load(
                path,             // Path to texture image
                &imgWidth,        // Pointer to image width
                &imgHeight,       // Pointer to image height
                &colorChannels,   // Pointer to color channels
                0
            );

            // If texture is successfully loaded
            if (tex_bytes) {
                std::cout << "Loaded successfully!" << std::endl;
                std::cout << "Binding texture..." << std::endl;

                // Prepare texture
                GLuint textureID;
                // texture starts at GL_TEXTURE0 upwards
                GLuint textureUnit = GL_TEXTURE0 + i;
                glGenTextures(1, &textureID);
                glActiveTexture(textureUnit);
                glBindTexture(GL_TEXTURE_2D, textureID);

                // If 3-channel texture (i.e., JPG)
                if (colorChannels == 3) {
                    std::cout << "3-channel image detected!" << std::endl;
                    glTexImage2D(
                        GL_TEXTURE_2D,
                        0,
                        GL_RGB,
                        imgWidth,
                        imgHeight,
                        0,
                        GL_RGB,
                        GL_UNSIGNED_BYTE,
                        tex_bytes
                    );
                }
                // If 4-channel texture (i.e., PNG)
                else {
                    std::cout << "4-channel image detected!" << std::endl;
                    glTexImage2D(
                        GL_TEXTURE_2D,
                        0,
                        GL_RGBA,
                        imgWidth,
                        imgHeight,
                        0,
                        GL_RGBA,
                        GL_UNSIGNED_BYTE,
                        tex_bytes
                    );
                }

                // Append the texture onto the model's list
                textures.push_back(Texture(textureID, textureUnit));
                glGenerateMipmap(GL_TEXTURE_2D);
                stbi_image_free(tex_bytes);

                std::cout << "Texture bound successfully!" << std::endl;
            }
            else {
                std::cout << "ERROR: Unable to load texture!" << std::endl;
            }
        }
    }

    // Loads the normal mappings of this model given a list of file paths.
    void loadNormalMap(std::string path) {
        // Flip images on load
        stbi_set_flip_vertically_on_load(true);

        std::cout << "Loading normal mapping from " << path << std::endl;

        // Load normal mapping from current path
        int imgWidth, imgHeight, colorChannels;
        unsigned char* norm_bytes = stbi_load(
            path.c_str(),             // texture image filename
            &imgWidth,        // pointer to image width
            &imgHeight,       // pointer to image height
            &colorChannels,   // point to color channels
            0                 // usually zero
        );

        // If normal mapping is successfully loaded
        if (norm_bytes) {
            std::cout << "Loaded successfully!" << std::endl;
            std::cout << "Binding normal mapping..." << std::endl;

            // Prepare normal mapping
            GLuint textureID;
            // normal mapping starts at GL_TEXTURE9 upwards (to avoid conflict with texture)
            GLuint textureUnit = GL_TEXTURE9;
            glGenTextures(1, &textureID);
            glActiveTexture(textureUnit);
            glBindTexture(GL_TEXTURE_2D, textureID);

            // Set the parameters to be the same as the diffuse color
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RGB,
                imgWidth,
                imgHeight,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                norm_bytes
            );

            // Instantiate normal mapping as Texture
            this->normalMap = Texture(textureID, textureUnit);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(norm_bytes);

            std::cout << "Normal mapping bound successfully!" << std::endl;
        }
        else {
            std::cout << "ERROR: Unable to load normal mapping!" << std::endl;
        }
    }

    // Binds this model's data onto its VAO and VBO.
    void bindObjData() {
        // Initialize data length and pointer offset for buffers
        // To accommodate models without normals, texcoords, and/or normal mapping
        this->dataLen = VERT_SIZE;
        int ptrOffset = VERT_SIZE;
        if (hasNormals)
            this->dataLen += NORM_SIZE;
        if (hasTexCoords)
            this->dataLen += UV_SIZE;
        if (hasNormalMapping)
            this->dataLen += TAN_SIZE + BITAN_SIZE;

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
            VERT_SIZE,
            GL_FLOAT,
            GL_FALSE,
            this->dataLen * sizeof(GL_FLOAT),
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
                NORM_SIZE,
                GL_FLOAT,
                GL_FALSE,
                this->dataLen * sizeof(GL_FLOAT),
                (void*)normPtr
            );

            // Enable index 1 (normals)
            glEnableVertexAttribArray(1);

            // Adjust pointer offset
            ptrOffset += NORM_SIZE;
        }

        // If the 3D Model has texture coordinates
        if (hasTexCoords) {
            // Tell OpenGL to use the next 2 data points as U and V
            GLintptr uvPtr = ptrOffset * sizeof(GLfloat);

            glVertexAttribPointer(
                2,
                UV_SIZE,
                GL_FLOAT,
                GL_FALSE,
                this->dataLen * sizeof(GL_FLOAT),
                (void*)uvPtr
            );

            // Enable index 2 (textures)
            glEnableVertexAttribArray(2);

            // Adjust pointer offset
            ptrOffset += UV_SIZE;
        }

        // If the 3D Model has normal mapping
        if (hasNormalMapping) {
            // Tell OpenGL to use the next 3 data points for the tangent
            GLintptr tangentPtr = ptrOffset * sizeof(float);
            // Tell OpenGL to use the next 3 data points for the bitangent
            GLintptr bitangentPtr = (ptrOffset + TAN_SIZE) * sizeof(float);

            glVertexAttribPointer(
                3,
                TAN_SIZE,
                GL_FLOAT,
                GL_FALSE,
                this->dataLen * sizeof(GL_FLOAT),
                (void*)tangentPtr
            );

            glVertexAttribPointer(
                4,
                BITAN_SIZE,
                GL_FLOAT,
                GL_FALSE,
                this->dataLen * sizeof(GL_FLOAT),
                (void*)bitangentPtr
            );

            // Enable index 3 (tangents)
            glEnableVertexAttribArray(3);
            // Enable index 4 (bitangents)
            glEnableVertexAttribArray(4);
        }

        // Reset buffer bindings
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }

public:
    // Instantiates a model object with texture and normal mapping.
    Model(
        std::string objPath,
        std::vector<std::string> texturePaths,
        std::string normalMapPath,
        glm::vec3 position = glm::vec3(0.0f),
        glm::vec3 rotation = glm::vec3(0.0f),
        glm::vec3 scale = glm::vec3(1.0f),
        glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f)
    ) {
        // Initialize attributes
        this->position = position;
        this->rotation = rotation;
        this->scale = scale;
        this->color = color;

        this->showColor = false;
        this->hasNormals = true;
        this->hasTexCoords = true;
        this->hasTexture = texturePaths.size() > 0 ? true : false;
        this->hasNormalMapping = normalMapPath.size() > 0 ? true : false;

        // Load the contents of the .obj file provided
        this->loadObjData(objPath);

        // If the model has textures, then load it
        if (hasTexture)
            this->loadTextures(texturePaths);

        // If the model has normal mapping, then load it
        if (hasNormalMapping)
            this->loadNormalMap(normalMapPath);

        // Finally, bind the object's data
        this->bindObjData();
    }

    // Instantiates a model object with textures only.
    Model(
        std::string objPath,
        std::vector<std::string> texturePaths,
        glm::vec3 position = glm::vec3(0.0f),
        glm::vec3 rotation = glm::vec3(0.0f),
        glm::vec3 scale = glm::vec3(1.0f),
        glm::vec3 color = glm::vec3(0.0f, 1.0f, 0.0f)
    ) {
        // Initialize attributes
        this->position = position;
        this->rotation = rotation;
        this->scale = scale;
        this->color = color;

        this->showColor = false;
        this->hasNormals = true;
        this->hasTexCoords = true;
        this->hasTexture = texturePaths.size() > 0 ? true : false;
        this->hasNormalMapping = false;

        // Load the contents of the .obj file provided
        this->loadObjData(objPath);

        // If the model has textures, then load it
        if (hasTexture)
            this->loadTextures(texturePaths);

        // Finally, bind the object's data
        this->bindObjData();
    }

    // Draw the model using the shader.
    void draw(Shader shader) {
        // Bind the model's VAO
        glBindVertexArray(this->VAO);

        // Compute for the transformation matrix; check if around world origin or not
        glm::mat4 transMatrix = computeTransMatrix();

        // Link the Model Matrix to the shader program
        shader.setMat4("model", transMatrix);

        // Tell the shader if this model uses normal mapping or not
        shader.setBool("hasNormalMapping", this->hasNormalMapping);

        // Tell the shader if this model uses texture/s or not
        shader.setBool("hasTexture", this->hasTexture);

        // Tel the shader if this model must use its color
        shader.setBool("showColor", this->showColor);

        // If the model has normal mapping, bind it
        if (this->hasNormalMapping) {
            // Bind the 3D model's normal mapping
            normalMap.bind();
            shader.setInt("norm_tex0", NORM_MAP_LOC);
        }

        // If the model has texture/s, bind it
        if (this->hasTexture && !this->showColor) {
            // Iterate through all possible texture units
            for (int i = 0; i < textures.size(); i++) {
                // Bind the 3D model's texture
                this->textures[i].bind();
                shader.setInt("tex" + std::to_string(i), i);
            }
        }
        // Else, use the model's color
        else {
            // Bind the 3D model's color
            shader.setVec3("modelColor", this->color);
        }

        // Draw the model itself
        glDrawArrays(GL_TRIANGLES, 0, (GLsizei)fullVertexData.size() / this->dataLen);
        glBindVertexArray(0);
    }

    // Helper function for computing the translation matrix of a 3D model.
    glm::mat4 computeTransMatrix() {
        // Initialize the Model Matrix as an identity matrix
        glm::mat4 transMatrix = glm::mat4(1.0f);

        // Apply a translation to the Model Matrix
        transMatrix = glm::translate(
            transMatrix,
            this->position
        );

        // Apply an Y-axis rotation to the Model Matrix
        transMatrix = glm::rotate(
            transMatrix,
            glm::radians(this->rotation.y),
            glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))
        );

        // Apply an X-axis rotation to the Model Matrix
        transMatrix = glm::rotate(
            transMatrix,
            glm::radians(this->rotation.x),
            glm::normalize(glm::vec3(1.0f, 0.0f, 0.0f))
        );

        // Apply an Z-axis rotation to the Model Matrix
        transMatrix = glm::rotate(
            transMatrix,
            glm::radians(this->rotation.z),
            glm::normalize(glm::vec3(0.0f, 0.0f, 1.0f))
        );

        // Apply scaling to the Model Matrix
        transMatrix = glm::scale(
            transMatrix,
            this->scale
        );

        return transMatrix;
    }

    // Toggles texture color usage; to use default texture color or green only.
    void toggleColor(bool use) {
        this->showColor = use;
    }

    // Return the position of the model.
    glm::vec3 getPosition() {
        return glm::vec3(this->position);
    }

    // Return the rotation of the model.
    glm::vec3 getRotation() {
        return glm::vec3(this->rotation);
    }

    // Return the scale of the model.
    glm::vec3 getScale() {
        return glm::vec3(this->scale);
    }

    // Return the color of the model.
    glm::vec3 getColor() {
        return glm::vec3(this->color);
    }

    // Set the position of the model.
    void setPosition(glm::vec3 newPosition) {
        this->position = newPosition;
    }

    // Set the rotation of the model.
    void setRotation(glm::vec3 newRotation) {
        this->rotation = newRotation;
    }

    // Set the scale of the model.
    void setScale(glm::vec3 newScale) {
        this->scale = newScale;
    }

    // Set the color of the model.
    void setColor(glm::vec3 newColor) {
        this->color = newColor;
    }
};