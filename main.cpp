/******** LIBRARIES FOR OPENGL GRAPHICS RENDERING ********/
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <string>       
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/******** LIBRARY FOR LOADING 3D MODELS ********/
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

/******** LIBRARY FOR LOADING TEXTURE IMAGES ********/
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/******** ADDITIONAL CLASSES ********/
#include "Classes/Shader.h"  // Shader Class
#include "Classes/Camera.h"  // Camera Class
#include "Classes/Light.h"   // Light Class
#include "Classes/Texture.h" // Texture Class
#include "Classes/Model.h"   // 3D Model Class
#include "Classes/Skybox.h"  // Skybox
#include "Classes/Player.h"  // Player Class

/******** 3D MODELS ********/
// Submarine (player) model, texture, and normal map paths
std::string submarineObjPath = "3D/Project/source/submarine.obj";
std::string submarineTexturePath = "3D/Project/textures/submarine/sublow0smooth_defaultmaterial_basecolor.png";
std::string submarineNormalMapPath = "3D/Project/textures/submarine/sublow0smooth_defaultmaterial_normal.png";

// Submarine initial configurations (position, rotation, scale)
glm::vec3 submarinePos = glm::vec3(0.0f, -10.0f, 0.0f);
glm::vec3 submarineRot = glm::vec3(0.0f);
glm::vec3 submarineScale = glm::vec3(0.05f);

// Vector of 3D enemy models and textures paths
std::vector<std::vector<std::string>> enemies{
    // Angler fish
    {"3D/Project/source/angler_fish.obj",
     "3D/Project/textures/angler_fish/Angler_Texture_V1.jpg"},

    // Stalker
    {"3D/Project/source/stalker.obj",
     "3D/Project/textures/stalker/stalker_low_unwrapped_1001_Diffuse.png"},

    // Peeper
    {"3D/Project/source/peeper.obj",
     "3D/Project/textures/peeper/texture.png"},

    // Reaper Leviathan
    {"3D/Project/source/leviathan_reaper.obj",
     "3D/Project/textures/leviathan_reaper/reaper_leviathan.png"},

    // Sea Emperor
    {"3D/Project/source/sea_emperor.obj",
     "3D/Project/textures/sea_emperor/sea_emperor_Diffuse.png"},

    // Hydra
    {"3D/Project/source/hydra.obj",
     "3D/Project/textures/hydra/Color_Hydra2.png"}
};

// Vector of 3D enemy model configurations (position, rotation, and scale)
std::vector<std::vector<glm::vec3>> enemyConfigs{
    // Angler fish
    {glm::vec3(0.0f, -98.0f, 8.0f),  // position
     glm::vec3(0.0f),                // rotation
     glm::vec3(1.0f)},               // scale

    // Stalker
    {glm::vec3(3.0f, -208.0f, -12.0f),
     glm::vec3(0.0f),
     glm::vec3(0.1f)},

    // Peeper
    {glm::vec3(8.0f, -150.0f, 10.0f),
     glm::vec3(0.0f),
     glm::vec3(0.05f)},

    // Reaper Leviathan
    {glm::vec3(-6.0f, -45.0f, 6.0f),
     glm::vec3(0.0f),
     glm::vec3(1.0f)},

    // Sea Emperor
    {glm::vec3(-3.0f, -179.0f, -15.0f),
     glm::vec3(0.0f),
     glm::vec3(0.005f)},

    // Hydra
    {glm::vec3(10.0f, -65.0f, -15.0f),
     glm::vec3(0.0f),
     glm::vec3(1.0f)},
};

/******** SKYBOX ********/
// Ocean depth/floor skybox faces
std::vector<std::string> whirlpoolSkyboxFaces{
    "Skybox/whirlpool/whirlpool_rt.png", // Right
    "Skybox/whirlpool/whirlpool_lf.png", // Left
    "Skybox/whirlpool/whirlpool_up.png", // Up
    "Skybox/whirlpool/whirlpool_dn.png", // Down
    "Skybox/whirlpool/whirlpool_ft.png", // Front
    "Skybox/whirlpool/whirlpool_bk.png"  // Back
};

/******** SHADERS ********/
// 3D model shader paths
const char* vertPath = "Shaders/main.vert";
const char* fragPath = "Shaders/main.frag";

// Skybox shader paths
const char* skyboxVertPath = "Shaders/skybox.vert";
const char* skyboxFragPath = "Shaders/skybox.frag";

/******** SYSTEM VARIABLES ********/
// Window dimensions
const int screenWidth = 750;
const int screenHeight = 750;

// Mouse inputs
bool firstMouseBtn = true;                // Avoids sudden camera movement on first mouse input
float prevX = (float)screenWidth / 2.0f;  // Previous mouse X offset
float prevY = (float)screenHeight / 2.0f; // Previous mouse Y offset
const float sensitivity = 0.1f;           // Camera sensitivity when receiving mouse input

/*
    Main (driver) function.
 */
int main(void) {
    // Initialize the GLFW library
    if (!glfwInit())
        return -1;

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "No Man's Submarine", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    gladLoadGL();

    /******** PREPARE SKYBOX ********/
    Skybox whirlpoolSkybox = Skybox(whirlpoolSkyboxFaces);

    /******** PREPARE SHADERS ********/
    Shader mainShaderProgram = Shader(vertPath, fragPath);               // 3D model shader
    Shader skyboxShaderProgram = Shader(skyboxVertPath, skyboxFragPath); // Skybox shader

    /******** PREPARE DIRECTIONAL LIGHT ********/
    DirectionalLight directionalLight = DirectionalLight(
        glm::vec3(0.0f, 1.0f, 0.0f), // light position
        glm::vec3(1.0f),             // light color
        glm::vec3(1.0f),             // ambient light color
        0.5f,                        // ambient light strength
        1.0f,                        // specular strength
        16.0f                        // specular phong
    );

    /******** PREPARE TOP VIEW CAMERA (ORTHOGRAPHIC) ********/
    OrthoCamera topViewCamera = OrthoCamera(
        glm::vec3(0.0f, 90.0f, 10.0f), // camera eye
        glm::vec3(0.0f, 0.0f, 1.0f),   // camera center
        glm::vec3(0.0f, 1.0f, 0.0f),   // camera up
        -1.0f,                         // left coordinate
        1.0f,                          // right coordinate
        -1.0f,                         // bottom coordinate
        1.0f,                          // top coordinate
        0.1f,                          // zNear
        100.0f                         // zFar
    );

    /******** PREPARE PLAYER ********/
    // Model
    std::vector<std::string> playerTextures{ submarineTexturePath };
    std::vector<std::string> playerNormalMaps{ submarineNormalMapPath };
    Model playerObj = Model(
        submarineObjPath,                   // path to .obj
        playerTextures,                     // list of textures
        playerNormalMaps,                   // list of normal mappings
        submarinePos,                       // position
        submarineRot,                       // rotation
        submarineScale                      // scale
    );
    // Point light
    glm::vec3 lightPos = submarinePos;
    lightPos.z += 10.0f; // Point light should be 10 distance units away from the front of the player's model
    PointLight pointLight = PointLight(
        lightPos,        // position
        glm::vec3(1.0f), // light color
        glm::vec3(1.0f), // ambient light color
        0.5f,            // ambient light strength
        1.0f,            // specular strength
        16.0f            // specular phong
    );
    // First POV Camera
    PerspectiveCamera firstPOVCamera = PerspectiveCamera(
        submarinePos,                               // same position as player model
        glm::vec3(0.0f, 1.0f, 0.0f),                // worldup
        glm::vec3(0.0f, 0.0f, 1.0f),                // camera center
        glm::radians(60.0f),                        // field of view
        (float)screenHeight / (float)screenWidth,   // aspect ratio
        0.1f,                                       // zNear
        100.0f                                      // zFar; can see farther unlike the third pov camera
    );
    // Third POV Camera
    glm::vec3 cameraPos = submarinePos;
    cameraPos.z += 10.0f; // Third POV camera should be 10 distance units away from the player's model
    PerspectiveCamera thirdPOVCamera = PerspectiveCamera(
        cameraPos,
        glm::vec3(0.0f, 1.0f, 0.0f),                // worldup
        glm::vec3(0.0f, 0.0f, 1.0f),                // camera center
        glm::radians(60.0f),                        // field of view
        (float)screenHeight / (float)screenWidth,   // aspect ratio
        0.1f,                                       // zNear
        100.0f                                      // zFar; third pov camera cannot see as far unlike the first pov camera
    );
    // Player
    Player player = Player(
        playerObj,       // model of player
        firstPOVCamera,  // first pov camera of player
        thirdPOVCamera,  // third pov camera of player
        pointLight       // point light of player's model
    );

    /******** PREPARE ENEMY MODELS ********/
    //std::vector<Model> enemyModels;
    //for (int i = 0; i < enemies.size(); i++) {
    //    // Get texture/s of current enemy model
    //    std::vector<std::string> enemyTextures;
    //    for (int j = 1; j < enemies[i].size(); j++) {
    //        enemyTextures.push_back(enemies[i][j]);
    //    }

    //    // Empty normal map list (since enemy models do not have normal maps)
    //    std::vector<std::string> emptyNormalMaps; 

    //    // Store current enemy model
    //    enemyModels.emplace_back(
    //        enemies[i][0],
    //        enemyTextures,
    //        emptyNormalMaps,
    //        enemyConfigs[i][0],
    //        enemyConfigs[i][1],
    //        enemyConfigs[i][2]
    //    );
    //}

    // Enable OpenGL's depth testing to avoid models "overlapping"
    // when using different colors or textures for each model
    glEnable(GL_DEPTH_TEST);

    //// For smooth fps movement in all devices
    //double prevFrameTime = 0.0f;
    //double theta_mod = 0.0f;

    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /******** RENDER SKYBOX ********/
        // Activate skybox shader
        skyboxShaderProgram.use();

        // Bind current POV camera to skybox shader
        // Check if current POV camera is player camera (1st or 3rd POV)
        if (player.isPOVCameraUsed()) {
            // Check if 1st or 3rd POV camera
            if (player.isFirstPOVCameraUsed()) {
                player.getFirstPOVCamera()->bindToShader(skyboxShaderProgram, true);
            }
            else {
                player.getThirdPOVCamera()->bindToShader(skyboxShaderProgram, true);
            }
        }
        // Bind top view camera
        else {
            topViewCamera.bindToShader(skyboxShaderProgram, true);
        }

        // Draw skybox
        whirlpoolSkybox.draw(skyboxShaderProgram);

        /******** RENDER MODEL ********/
        // Activate model shader
        mainShaderProgram.use();

        // Bind directional light to shader
        directionalLight.bindToShader(mainShaderProgram);

        // Bind top view camera to shader if player POV camera is not currently used
        if (!player.isPOVCameraUsed()) {
            topViewCamera.bindToShader(mainShaderProgram);
        }

        // Render player model
        player.draw(mainShaderProgram);

        // Render enemy models
        /*for (int i = 0; i < enemyModels.size(); i++) {
            enemyModels[i].draw(mainShaderProgram);
        }*/

        //// Delta time for uniformity in rotation across machines
        //double currentTime = glfwGetTime();
        //double deltaTime = currentTime - prevFrameTime;
        //prevFrameTime = currentTime;
        //theta_mod -= 20.0f * (float)deltaTime;

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