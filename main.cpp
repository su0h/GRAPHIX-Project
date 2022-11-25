#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <string>       
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// import additional classes
#include "Classes/Shader.h"  // Shader Class
#include "Classes/Model.h"   // 3D Model Class
#include "Classes/Camera.h"  // Camera Class
#include "Classes/Light.h"   // Light Class

/******** CONFIGURATION VARIABLES ********/
// Window Dimensions
const int screenWidth = 750;
const int screenHeight = 750;

enum Skybox {
    DEFAULT, 
    AWUP, 
    WHIRLPOOL
};
Skybox activeSkybox = DEFAULT;

// Shader Paths
const char* vertPath = "Shaders/main.vert";
const char* fragPath = "Shaders/main.frag";
const char* skyboxVertPath = "Shaders/skybox.vert";
const char* skyboxFragPath = "Shaders/skybox.frag";

// Object Path
const char* submarineObjPath = "3D/Project/source/sublow0smooth.obj";

// Texture Paths
const char* submarineTexturePath = "3D/Project/textures/submarine/sublow0smooth_defaultmaterial_basecolor.png";
const char* submarineNormalMapPath = "3D/Project/textures/submarine/sublow0smooth_defaultmaterial_normal.png";

// Skybox Faces
// Skybox Source: https://web.archive.org/web/20191205162114/http://www.custommapmakers.org:80/skyboxes.php
std::string skyboxFaces[]{
       "Skybox/awup/awup_rt.png", // Right
       "Skybox/awup/awup_lf.png", // Left
       "Skybox/awup/awup_up.png", // Up
       "Skybox/awup/awup_dn.png", // Down
       "Skybox/awup/awup_ft.png", // Front
       "Skybox/awup/awup_bk.png"  // Back
};

//std::string skyboxFaces[]{
//       "Skybox/whirlpool/whirlpool_rt.png", // Right
//       "Skybox/whirlpool/whirlpool_lf.png", // Left
//       "Skybox/whirlpool/whirlpool_up.png", // Up
//       "Skybox/whirlpool/whirlpool_dn.png", // Down
//       "Skybox/whirlpool/whirlpool_ft.png", // Front
//       "Skybox/whirlpool/whirlpool_bk.png"  // Back
//};
//
//std::string skyboxFaces[]{
//       "Skybox/default/rainbow_rt.png", // Right
//       "Skybox/default/rainbow_lf.png", // Left
//       "Skybox/default/rainbow_up.png", // Up
//       "Skybox/default/rainbow_dn.png", // Down
//       "Skybox/default/rainbow_ft.png", // Front
//       "Skybox/default/rainbow_bk.png"  // Back
//};


int main(void) {
    // Initialize the GLFW library
    if (!glfwInit())
        return -1;

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Andre Ponce", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    gladLoadGL();

    /******** PREPARE SHADERS ********/
    Shader mainShaderProgram = Shader(vertPath, fragPath);
    Shader skyboxShaderProgram = Shader(skyboxVertPath, skyboxFragPath);

    /******** PREPARE CUBEMAP ********/
    // Vertices for the cube
    float skyboxVertices[]{
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
    unsigned int skyboxIndices[]{
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
    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);

    // Bind Skybox VAO and VBO
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(skyboxVertices),
        &skyboxVertices,
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GL_INT) * 36,
        &skyboxIndices,
        GL_STATIC_DRAW
    );

    // Enable Skybox vertex attribute array
    glEnableVertexAttribArray(0);

    // Prepare Skybox texture
    unsigned int skyboxTexture;
    glGenTextures(1, &skyboxTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

    // Prevents pixelating if too close or far
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Prevents tiling
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Load the skybox faces
    for (unsigned int i = 0; i < 6; i++) {
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

    // Reset this to true
    stbi_set_flip_vertically_on_load(true);

    /******** PREPARE LIGHT ********/
    // Light attributes
    glm::vec3 lightPos = glm::vec3(-4, 7, 5);
    glm::vec3 lightColor = glm::vec3(1.0f);
    glm::vec3 ambientColor = lightColor;
    float ambientStr = 0.5f;
    float specStr = 1.0f;
    float specPhong = 16.0f;

    // Create Point Light
    PointLight pointLight = PointLight(
        lightPos,       // position
        lightColor,     // light color
        ambientColor,   // ambient light color
        ambientStr,     // ambient light strength
        specStr,        // specular strength
        specPhong       // specular phong
    );

    /******** PREPARE CAMERA ********/
    PerspectiveCamera perspectiveCamera = PerspectiveCamera(
        glm::vec3(0.0f, 0.0f, 20.0f),               // position
        glm::vec3(0.0f, 1.0f, 0.0f),                // worldup
        glm::vec3(0.0f, 0.0f, 0.0f),                // camera center
        glm::radians(60.0f),                        // field of view
        (float)screenHeight / (float)screenWidth,   // aspect ratio
        0.1f,                                       // zNear
        100.0f                                      // zFar
    );

    /******** PREPARE MODEL ********/
    // List of texture paths
    std::vector<std::string> submarineTextures;
    // Yae Texture
    submarineTextures.push_back(submarineTexturePath);

    // List of normal mapping paths
    std::vector<std::string> submarineNormalMaps;
    // Brick Wall Normal Mapping
    submarineNormalMaps.push_back(submarineNormalMapPath);

    // Create the plane object
    Model planeObj = Model(
        submarineObjPath,                   // path to .obj
        submarineTextures,                  // list of textures
        submarineNormalMaps,                // list of normal mappings
        glm::vec3(0.0f),                    // position
        glm::vec3(0.0f, 0.0f, 0.0f),        // rotation
        glm::vec3(0.05f)                    // scale
    );

    // Enable OpenGL's depth testing to avoid models "overlapping"
    // when using different colors for each model
    glEnable(GL_DEPTH_TEST);

    // for smooth fps movement in all devices
    double prevFrameTime = 0.0f;
    double theta_mod = 0.0f;

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /******** RENDER SKYBOX ********/
        glEnable(GL_BLEND);

        // Default blending function
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);

        // Activate skybox shader
        skyboxShaderProgram.use();

        // Bind skybox attributes
        glm::mat4 sky_view = glm::mat4(1.0f);
        sky_view = glm::mat4(glm::mat3(perspectiveCamera.computeViewMatrix()));
        skyboxShaderProgram.setMat4("projection", perspectiveCamera.computeProjectionMatrix());
        skyboxShaderProgram.setMat4("view", sky_view);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTexture);

        // Draw skybox
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);

        // Reset deppth mask and depth function
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);

        /******** RENDER MODEL ********/
        // Activate model shader
        mainShaderProgram.use();

        // Bind point light to shader
        pointLight.bindToShader(mainShaderProgram);

        // Bind camera to shader
        perspectiveCamera.bindToShader(mainShaderProgram);

        // Delta Time for uniformity in rotation across machines
        double currentTime = glfwGetTime();
        double deltaTime = currentTime - prevFrameTime;
        prevFrameTime = currentTime;
        theta_mod -= 20.0f * (float)deltaTime;

        // Adjust rotation of object
        planeObj.setRotation(glm::vec3(theta_mod, 0.0f, 0.0f));

        // Draw plane object
        planeObj.draw(mainShaderProgram);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }
    // Some clean up (OPTIONAL, but recommended)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glfwTerminate();
    return 0;
}