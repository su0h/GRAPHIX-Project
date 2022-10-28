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
#include "Classes/ShaderClass.h"  // Shader Class
#include "Classes/Model3DClass.h" // 3D Model Class

/******** CONFIGURATION VARIABLES ********/
// Window Dimensions
const int screenWidth = 750;
const int screenHeight = 750;

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

    // Enable OpenGL's depth testing to avoid models "overlapping"
    // when using different colors for each model
    glEnable(GL_DEPTH_TEST);
    // Loop until the user closes the window
    while (!glfwWindowShouldClose(window)) {
        // Render here
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // TODO: Place rendering code here...

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