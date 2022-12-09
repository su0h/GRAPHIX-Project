/******** LIBRARIES FOR OPENGL GRAPHICS RENDERING ********/
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <math.h>
#include <string>       
#include <iostream>
#include <iomanip>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/******** LIBRARY FOR LOADING 3D MODELS ********/
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

/******** LIBRARY FOR LOADING TEXTURE IMAGES ********/
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

/******** OPENGL TEXT RENDERING ********/
// Taken from: https://github.com/capnramses/opengl_text_rendering
// *approved by sir for use as well
#include "Text/text.cpp"

/******** ADDITIONAL CLASSES ********/
#include "Classes/Shader.h"  // Shader Class
#include "Classes/Camera.h"  // Camera, PerspectiveCamera, OrthoCamera Classes
#include "Classes/Light.h"   // Light, PointLight, DirectionalLight Classes
#include "Classes/Texture.h" // Texture Class
#include "Classes/Model.h"   // 3D Model Class
#include "Classes/Skybox.h"  // Skybox Class
#include "Classes/Player.h"  // Player Class

/******** 3D MODELS ********/
// Submarine (player) model, texture, and normal map paths
std::string submarineObjPath = "3D/Project/source/submarine.obj";
std::string submarineTexturePath = "3D/Project/textures/submarine/sublow0smooth_defaultmaterial_basecolor.png";
std::string submarineNormalMapPath = "3D/Project/textures/submarine/sublow0smooth_defaultmaterial_normal.png";

// Submarine initial configurations (position, rotation, scale)
glm::vec3 submarinePos = glm::vec3(0.0f, -100.0f, 0.0f);
glm::vec3 submarineRot = glm::vec3(0.0f, 0.0f, 0.0f);
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
    {glm::vec3(290.0f, -1000.0f, -20.0f), // position
     glm::vec3(0.0f, -40.0f, 0.0f),       // rotation
     glm::vec3(20.0f)},                   // scale

    // Stalker
    {glm::vec3(270.0f, -300.0f, -250.0f),
     glm::vec3(0.0f, 30.0f, 0.0f),
     glm::vec3(0.1f)},

    // Peeper
    {glm::vec3(300.0f, -10.0f, 250.0f),
     glm::vec3(0.0f, -75.0f, 0.0f),
     glm::vec3(0.025f)},

    // Reaper Leviathan
    {glm::vec3(-105.0f, -500.0f, -155.0f),
     glm::vec3(0.0f, 65.0f, 0.0f),
     glm::vec3(1.0f)},

    // Sea Emperor
    {glm::vec3(-200.0f, -1500.0f, 215.0f),
     glm::vec3(0.0f, -150.0f, 0.0f),
     glm::vec3(0.1f)},

    // Hydra
    {glm::vec3(0.0f, -98.0f, 25.0f),
     glm::vec3(0.0f, 180.0f, 0.0f),
     glm::vec3(1.0f)}
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

/*
    Main (driver) function.
 */
int main(void) {
    // Initialize the GLFW library
    if (!glfwInit())
        return -1;

    // Window dimensions
    const int screenWidth = 900;
    const int screenHeight = 900;

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
    Shader skyboxShaderProgram = Shader(skyboxVertPath, skyboxFragPath); // skybox shader

    /******** PREPARE DIRECTIONAL LIGHT ********/
    DirectionalLight directionalLight = DirectionalLight(
        glm::vec3(0.0f, 1.0f, 0.0f), // light position
        glm::vec3(1.0f),             // light color
        glm::vec3(1.0f),             // ambient light color
        0.05f,                       // ambient light strength
        1.0f,                        // specular strength
        16.0f                        // specular phong
    );

    /******** PREPARE TOP VIEW / BIRD'S EYE CAMERA (ORTHOGRAPHIC) ********/
    // Top view / bird's eye view camera
    OrthoCamera topViewCamera = OrthoCamera(
        glm::vec3(submarinePos.x, 90.0f, submarinePos.z), // camera eye
        glm::vec3(0.0f, 0.0f, 1.0f),                      // camera up
        submarinePos,                                     // camera center
        -50.0f,                                           // left coordinate
        50.0f,                                            // right coordinate
        -50.0f,                                           // bottom coordinate
        50.0f,                                            // top coordinate
        0.1f,                                             // zNear
        10000.0f                                          // zFar
    );
    // Top / bird's eye (orthographic) view variables
    float topViewSpeed = 0.5f; // top view camera movement speed

    /******** PREPARE PLAYER ********/
    // Model
    std::vector<std::string> playerTextures{ submarineTexturePath };
    Model playerObj(
        submarineObjPath,       // path to .obj
        playerTextures,         // list of textures
        submarineNormalMapPath, // path to normal map texture
        submarinePos,           // position
        submarineRot,           // rotation
        submarineScale          // scale
    );

    // Point light
    PointLight pointLight = PointLight(
        submarinePos,    // position
        glm::vec3(1.0f), // light color
        glm::vec3(1.0f), // ambient light color
        0.5f,            // ambient light strength
        1.0f,            // specular strength
        16.0f            // specular phong
    );

    // 1st POV camera
    glm::vec3 firstPOVCameraPos = submarinePos;     // same position as player's model
    PerspectiveCamera firstPOVCamera = PerspectiveCamera(
        firstPOVCameraPos,                          // camera position
        glm::vec3(0.0f, 1.0f, 0.0f),                // worldup
        glm::vec3(0.0f, 0.0f, 0.0f),                // camera center
        glm::radians(60.0f),                        // field of view
        (float)screenHeight / (float)screenWidth,   // aspect ratio
        0.1f,                                       // zNear
        1000.0f                                     // zFar; can see farther unlike the third pov camera
    );
    firstPOVCamera.setCenter(0.0f, 90.0f);          // initially rotate 1st POV camera 90 degrees to the right
                                                    // For it to be facing as well to where the player's model is facing
    // 3rd POV Camera
    glm::vec3 thirdPOVCameraPos = submarinePos;
    PerspectiveCamera thirdPOVCamera = PerspectiveCamera(
        thirdPOVCameraPos,
        glm::vec3(0.0f, 1.0f, 0.0f),              
        submarinePos, // point to player's model                              
        glm::radians(60.0f),                        
        (float)screenHeight / (float)screenWidth,   
        0.1f,                                       
        250.0f        // zFar; third pov camera cannot see as far unlike the first pov camera
    );

    // Player
    Player player = Player(
        &playerObj,       // model of player
        &firstPOVCamera,  // first pov camera of player
        &thirdPOVCamera,  // third pov camera of player
        &pointLight       // point light of player's model
    );

    /******** PREPARE ENEMY MODELS ********/
    std::vector<Model> enemyModels;
    for (int i = 0; i < enemies.size(); i++) {
        // Get texture/s of current enemy model
        std::vector<std::string> enemyTextures;
        for (int j = 1; j < enemies[i].size(); j++) {
            enemyTextures.push_back(enemies[i][j]);
        }

        // Store current enemy model
        enemyModels.emplace_back(
            enemies[i][0],
            enemyTextures,
            enemyConfigs[i][0],
            enemyConfigs[i][1],
            enemyConfigs[i][2]
        );
    }

    // Mouse input variables for player's 3rd POV camera
    bool firstMove = true;                            // Avoids sudden camera movement on first mouse input
    float thirdPOVPrevX = (float)screenWidth / 2.0f;  // Previous mouse X offset
    float thirdPOVPrevY = (float)screenHeight / 2.0f; // Previous mouse Y offset

    // Mouse input variables for top view camera (drag control)
    bool firstClick = true; // Determines if its the left mouse button click that initializes the drag control
    float topViewPrevX = (float)screenWidth / 2.0f;
    float topViewPrevY = (float)screenHeight / 2.0f;

    // Camera sensitivity when receiving mouse input
    const float sensitivity = 0.1f;                   

    // Previous swap time between player's 1st and 3rd POV cameras; for cooldown
    float prevCamSwapTime = 0.0f;

    // Previous swap time of light's intensity
    float prevIntSwapTime = 0.0f;

    // Enable OpenGL's depth testing to avoid models "overlapping"
    // when using different colors or textures for each model
    glEnable(GL_DEPTH_TEST);

    // Initialize OpenGL text rendering
    init_text_rendering("Text/freemono.png", "Text/freemono.meta", screenWidth, screenHeight);

    // Text attributes
    float x = -0.95f;
    float y = 1.0f;
    float size_px = 40.0f;
    int r, g, b, a;
    r = g = b = a = 1.0f;

    // Create the text itself (this returns a text ID for referencing later on)
    int depthCtrID = add_text(
        "DEPTH: 0.00",      // default text
        x,                  // x-axis position [-1.0, 1.0]
        y,                  // y-axis position [-1.0, 1.0f]
        size_px,            // text size in pixels (i.e., 100.0f)
        r,                  // red channel value of text color
        g,                  // green channel value of text color
        b,                  // blue channel value of text color
        a                   // alpha channel value of text color
    );

    while (!glfwWindowShouldClose(window)) {
        // Clear color and depth buffer per iteration
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /******** RENDER SKYBOX ********/
        // Use skybox shader program
        skyboxShaderProgram.use();

        // Bind current POV camera to skybox shader
        // Check if current POV camera is player camera (1st or 3rd POV)
        if (player.isPOVCameraUsed()) {
            // Check if 1st or 3rd POV camera
            if (player.isFirstPOVCameraUsed()) {
                // Render skybox with shade of green for first POV
                whirlpoolSkybox.toggleColor(true);
                player.getFirstPOVCamera()->bindToShaderFirstPOV(skyboxShaderProgram, true);
            }
            else {
                // Render skybox with default texture color
                whirlpoolSkybox.toggleColor(false);
                player.getThirdPOVCamera()->bindToShader(skyboxShaderProgram, true);
            }
        }
        else {
            whirlpoolSkybox.toggleColor(false);
            topViewCamera.bindToShader(skyboxShaderProgram, true);
        }

        // Draw skybox
        whirlpoolSkybox.draw(skyboxShaderProgram);

        /******** RENDER MODEL ********/
        // Use main (model) shader program
        mainShaderProgram.use();

        // Bind top view camera to main shader if player's POV camera is currently not used
        if (!player.isPOVCameraUsed()) {
            topViewCamera.bindToShader(mainShaderProgram);
        }

        // If first POV camera is currently used
        if (player.isPOVCameraUsed() && player.isFirstPOVCameraUsed()) {
            // Render enemy models with shade of green for first POV
            for (int i = 0; i < enemyModels.size(); i++) {
                enemyModels[i].toggleColor(true);
            }
        }
        else {
            // Third POV or top view camera is used, render enemy models with default texture color
            for (int i = 0; i < enemyModels.size(); i++) {
                enemyModels[i].toggleColor(false);
            }
        }

        // Bind directional light to shader
        directionalLight.bindToShader(mainShaderProgram);

        // Draw player model
        player.draw(mainShaderProgram);

        // Draw enemy models
        for (int i = 0; i < enemyModels.size(); i++) {
            enemyModels[i].draw(mainShaderProgram);
        }

        // Update the text (that was created a while ago) with the current player submarine depth value
        float playerDepth = player.getModel()->getPosition().y;
        std::stringstream stream; // To limit depth value to two decimal places
        stream << std::fixed << std::setprecision(2) << playerDepth;
        std::string formattedPlayerDepth = "DEPTH: " + stream.str();
        update_text(depthCtrID, formattedPlayerDepth.c_str());

        // Draw all texts (in this case, only the depth text)
        draw_texts();

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events (inputs)
        glfwPollEvents();

        /******** MOUSE INPUTS ********/
        // If player's 3rd POV camera is currently used
        if (player.isPOVCameraUsed() && !player.isFirstPOVCameraUsed()) {
            // Get current mouse X and Y coordinates
            double posX; 
            double posY;
            glfwGetCursorPos(window, &posX, &posY);

            // If first mouse cursor input for the window instance
            if (firstMove || thirdPOVPrevX != posX || thirdPOVPrevY != posY) {
                if (firstMove) {
                    // Set current mouse cursor X and Y coordinates as previous coordinates
                    thirdPOVPrevX = posX;
                    thirdPOVPrevY = posY;

                    // To avoid sudden camera movements
                    firstMove = false;
                }

                // Get the movement offset between the last and current frame
                // Adjusted with sensitivity
                float offsetX = (posX - thirdPOVPrevX) * sensitivity;
                float offsetY = (thirdPOVPrevY - posY) * sensitivity;

                // Set current coordinates as previous coordinates for succeeding mouse inputs
                thirdPOVPrevX = posX;
                thirdPOVPrevY = posY;

                // Rotate 3rd pov camera
                player.rotateThirdPOVCameraOnMouse(offsetX, offsetY);
            }
        }
        // If top view camera is currently used
        else if (!player.isPOVCameraUsed()) {
            // If left mouse button is currently clicked and hold
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
                // Get current mouse X and Y coordinates
                double posX;
                double posY;
                glfwGetCursorPos(window, &posX, &posY);

                // If first left mouse click and hold; drag control initialized
                if (firstClick) {
                    // Set current mouse cursor X and Y coordinates as previous coordinates
                    topViewPrevX = posX;
                    topViewPrevY = posY;

                    // To avoid sudden camera movements
                    firstClick = false;
                }

                // Get the movement offset between the last and current frame
                // Adjusted with sensitivity; negative for opposite direction because of dragging
                float offsetX = (topViewPrevX - posX) * -sensitivity;
                float offsetY = (topViewPrevY - posY) * -sensitivity;

                // Set current coordinates as previous coordinates for succeeding mouse inputs
                topViewPrevX = posX;
                topViewPrevY = posY;

                // Move top view camera
                glm::vec3 topViewCamPos = topViewCamera.getPosition();
                glm::vec3 topViewCamCenter = topViewCamera.getCenter();
                topViewCamPos.x += offsetX;
                topViewCamCenter.x += offsetX;
                topViewCamPos.z += offsetY;
                topViewCamCenter.z += offsetY;
                topViewCamera.setPosition(topViewCamPos);
                topViewCamera.setCenter(topViewCamCenter);
            }
            else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
                // Reset first left mouse button click indicator for succeeding inputs
                firstClick = true;
            }
        }

        /******** KEYBOARD INPUTS ********/
        // Toggle between First Person and Third Person POV camera
        if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
            // Get current time 
            double currTime = glfwGetTime();

            // If 0.2 seconds have passed, switch camera POVs
            if (currTime - prevCamSwapTime > 0.2f) {
                // Check first if top view camera was currently used
                if (!player.isPOVCameraUsed()) {
                    // Change back to using player POV camera
                    player.toggleCamera(true);
                }
                else {
                    // If 1st POV camera is going to be used
                    if (!player.isFirstPOVCameraUsed()) {
                        // Use 1st POV camera of player
                        player.toggleFirstPOVCamera(true);
                    }
                    else {
                        // Use 3rd POV camera of a player
                        player.toggleFirstPOVCamera(false);
                    }
                }

                // Make current time as previous time for succeeding inputs
                prevCamSwapTime = currTime;
            }
        }

        // Toggle Top View camera
        if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
            // Update position and direction of top view camera based on new position of player's model
            // Ensures that its on the top center of the player's model when switched back
            glm::vec3 topViewCameraPos = topViewCamera.getPosition();
            topViewCameraPos.x = player.getModel()->getPosition().x;
            topViewCameraPos.z = player.getModel()->getPosition().z;
            topViewCamera.setPosition(topViewCameraPos);
            topViewCamera.setCenter(player.getModel()->getPosition());

            // Swap between player POV camera and top view camera
            player.toggleCamera(false);
        }

        // Move player submarine forward
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            // If currently used camera view is top view
            if (!player.isPOVCameraUsed()) {
                // Move top view camera up
                glm::vec3 topViewCamPos = topViewCamera.getPosition();
                glm::vec3 topViewCamCenter = topViewCamera.getCenter();
                topViewCamPos.z += topViewSpeed;
                topViewCamCenter.z += topViewSpeed;
                topViewCamera.setPosition(topViewCamPos);
                topViewCamera.setCenter(topViewCamCenter);
            }
            else {
                // Move player forward
                player.moveForward();
            }
        }

        // Move player submarine backward
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            if (!player.isPOVCameraUsed()) {
                // Move top view camera down
                glm::vec3 topViewCamPos = topViewCamera.getPosition();
                glm::vec3 topViewCamCenter = topViewCamera.getCenter();
                topViewCamPos.z -= topViewSpeed;
                topViewCamCenter.z -= topViewSpeed;
                topViewCamera.setPosition(topViewCamPos);
                topViewCamera.setCenter(topViewCamCenter);
            }
            else {
                // Move player backwards
                player.moveBackwards();
            }
        }

        // Rotate player submarine counterclockwise
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            if (!player.isPOVCameraUsed()) {
                // Move top view camera to the left
                glm::vec3 topViewCamPos = topViewCamera.getPosition();
                glm::vec3 topViewCamCenter = topViewCamera.getCenter();
                topViewCamPos.x += topViewSpeed;
                topViewCamCenter.x += topViewSpeed;
                topViewCamera.setPosition(topViewCamPos);
                topViewCamera.setCenter(topViewCamCenter);
            }
            else {
                // Turn player to the left
                player.turnLeft();
            }
        }

        // Rotate player submarine clockwise
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            if (!player.isPOVCameraUsed()) {
                // Move top view camera to the right
                glm::vec3 topViewCamPos = topViewCamera.getPosition();
                glm::vec3 topViewCamCenter = topViewCamera.getCenter();
                topViewCamPos.x -= topViewSpeed;
                topViewCamCenter.x -= topViewSpeed;
                topViewCamera.setPosition(topViewCamPos);
                topViewCamera.setCenter(topViewCamCenter);
            }
            else {
                // Turn player to the right
                player.turnRight();
            }
        }

        // Make player submarine ascend
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            // If player POV camera is currently used
            if (player.isPOVCameraUsed()) {
                // Ascend player
                player.ascend();

                // Print current depth of player's model
                std::cout << "DEPTH: " << player.getModel()->getPosition().y << std::endl;
            }
        }

        // Make player submarine descend
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            if (player.isPOVCameraUsed()) {
                // Descend player
                player.descend();

                std::cout << "DEPTH: " << player.getModel()->getPosition().y << std::endl;
            }
        }

        // Toggle point light intensity
        if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
            // Get current time
            double currTime = glfwGetTime();

            // If 0.2 seconds has already passed
            if (currTime - prevIntSwapTime > 0.2f) {
                // Change light intensity between low, medium, and high
                player.changeLightIntensity();

                // Assign current time as previous time for succeeding inputs
                prevIntSwapTime = currTime;
            }
        }
    }

    // Some clean up (OPTIONAL, but recommended)
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glfwTerminate();
    return 0;
}