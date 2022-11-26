#pragma once

/*
	Skybox class implementation. Holds every skybox-related functionality.
*/
class Skybox {
private:
    GLuint VAO;
    Texture texture;

    // Initializes the cubemap of this skybox
    void initCubemap() {
        // Vertices for the cube
        float vertices[]{
            -1.f, -1.f, 1.f, // 0
            1.f, -1.f, 1.f,  // 1
            1.f, -1.f, -1.f, // 2
            -1.f, -1.f, -1.f,// 3
            -1.f, 1.f, 1.f,  // 4
            1.f, 1.f, 1.f,   // 5
            1.f, 1.f, -1.f,  // 6
            -1.f, 1.f, -1.f  // 7
        };

        // Skybox Indices
        unsigned int indices[]{
            1,2,6,
            6,5,1,

            0,4,7,
            7,3,0,

            4,5,6,
            6,7,4,

            0,3,2,
            2,1,0,

            0,1,5,
            5,4,0,

            3,7,6,
            6,2,3
        };

        // Prepare Skybox for binding
        unsigned int VBO, EBO;
        glGenVertexArrays(1, &this->VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        // Bind Skybox VAO and VBO
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(
            GL_ARRAY_BUFFER,
            sizeof(vertices),
            &vertices,
            GL_STATIC_DRAW
        );
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            3 * sizeof(GL_FLOAT),
            (void*)0
        );

        // Construct Skybox EBO
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            sizeof(GL_INT) * 36,
            &indices,
            GL_STATIC_DRAW
        );

        // Enable Skybox vertex attribute array
        glEnableVertexAttribArray(0);
    }
 
    // Loads the textures of this model given a list of file paths.
    void loadTextures(std::vector<std::string> skyboxFaces) {
        // Prepare Skybox texture
        unsigned int texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

        // Prevents pixelating if too close or far
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // Prevents tiling
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        for (int i = 0; i < 6; i++) {
            // Temporarily set this to false
            stbi_set_flip_vertically_on_load(false);

            // Load texture data
            int width, height, skyboxColorChannel;
            unsigned char* data = stbi_load(
                skyboxFaces[i].c_str(),
                &width,
                &height,
                &skyboxColorChannel,
                0
            );

            // If loaded successfully
            if (data) {
                // Bind the texture
                glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                    0,
                    GL_RGB,
                    width,
                    height,
                    0,
                    GL_RGB,
                    GL_UNSIGNED_BYTE,
                    data
                );
            }

            // Some cleanup
            stbi_image_free(data);
        }

        // Instantiate Texture object of this skybox
        this->texture = Texture(texture, GL_TEXTURE0);

        // Reset this to true
        stbi_set_flip_vertically_on_load(true);
    }
public:
    // Instantiates a Skybox object.
	Skybox(std::vector<std::string> skyboxFaces) {
        // Prepare cubemap
        initCubemap();

        // Load skybox textures
        loadTextures(skyboxFaces);
	}

    // Draw the skybox using the shader.
    void draw(Shader shader) {
        glEnable(GL_BLEND);

        // Default blending function
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        // Bind the skybox VAO to the shader
        glBindVertexArray(this->VAO);
        this->texture.bind();

        // Draw skybox
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Reset deppth mask and depth function
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);
    }
};