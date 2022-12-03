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
#include "Classes/Camera.h"  // Camera Class
#include "Classes/Light.h"   // Light Class
#include "Classes/Texture.h" // Texture Class
#include "Classes/Model.h"   // 3D Model Class
#include "Classes/Skybox.h"  // Skybox
#include "Classes/Player.h"  // Player Class

/******** CONFIGURATION VARIABLES ********/
// Window Dimensions
const int screenWidth = 750;
const int screenHeight = 750;

// Shader Paths
const char* vertPath = "Shaders/main.vert";
const char* fragPath = "Shaders/main.frag";
const char* skyboxVertPath = "Shaders/skybox.vert";
const char* skyboxFragPath = "Shaders/skybox.frag";

// Object Path
const char* submarineObjPath = "3D/Project/source/submarine.obj";

// Texture Paths
const char* submarineTexturePath = "3D/Project/textures/submarine/sublow0smooth_defaultmaterial_basecolor.png";
const char* submarineNormalMapPath = "3D/Project/textures/submarine/sublow0smooth_defaultmaterial_normal.png";

// Skybox Faces
// Skybox Source: https://web.archive.org/web/20191205162114/http://www.custommapmakers.org:80/skyboxes.php
std::vector<std::string> defaultSkyboxFaces{
    "Skybox/default/rainbow_rt.png", // Right
    "Skybox/default/rainbow_lf.png", // Left
    "Skybox/default/rainbow_up.png", // Up
    "Skybox/default/rainbow_dn.png", // Down
    "Skybox/default/rainbow_ft.png", // Front
    "Skybox/default/rainbow_bk.png"  // Back
};

std::vector<std::string> whirlpoolSkyboxFaces{
    "Skybox/whirlpool/whirlpool_rt.png", // Right
    "Skybox/whirlpool/whirlpool_lf.png", // Left
    "Skybox/whirlpool/whirlpool_up.png", // Up
    "Skybox/whirlpool/whirlpool_dn.png", // Down
    "Skybox/whirlpool/whirlpool_ft.png", // Front
    "Skybox/whirlpool/whirlpool_bk.png"  // Back
};

std::vector<std::string> awupSkyboxFaces{
    "Skybox/awup/awup_rt.png", // Right
    "Skybox/awup/awup_lf.png", // Left
    "Skybox/awup/awup_up.png", // Up
    "Skybox/awup/awup_dn.png", // Down
    "Skybox/awup/awup_ft.png", // Front
    "Skybox/awup/awup_bk.png"  // Back
};


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

    /******** PREPARE SKYBOX ********/
    Skybox defaultSkybox = Skybox(defaultSkyboxFaces);
    //Skybox awupSkybox = Skybox(awupSkyboxFaces);
    //Skybox whirlpoolSkybox = Skybox(whirlpoolSkyboxFaces);

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
    Model submarineObj = Model(
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

    // Toggle skybox color
    defaultSkybox.toggleColor();

    // Toggle object color
    submarineObj.toggleColor();

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /******** RENDER SKYBOX ********/
        glEnable(GL_BLEND);

        // Default blending function
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // Activate skybox shader
        skyboxShaderProgram.use();

        // Bind camera to skybox shader
        perspectiveCamera.bindToShader(skyboxShaderProgram, true);

        // Draw skybox
        defaultSkybox.draw(skyboxShaderProgram);

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
        submarineObj.setRotation(glm::vec3(theta_mod, 0.0f, 0.0f));

        // Draw submarine object
        submarineObj.draw(mainShaderProgram);

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