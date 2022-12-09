#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

/*
    Shader class implementation. Holds every shader-related functionality.
    
    Adapted from: https://learnopengl.com/code_viewer_gh.php?code=includes/learnopengl/shader_m.h
 */
class Shader {
private:
    // Unique identifier for this shader
    unsigned int shaderProgramID;

    /******** UTILITY FUNCTIONS ********/
    // Check for shader compilation errors.
    void checkCompileErrors(GLuint shader, std::string type) {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "-----------\n" << std::endl;
            }
        }
        else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "-----------\n" << std::endl;
            }
        }
    }

public:
    // Instantiates a Shader object.
    Shader(const char* vertPath, const char* fragPath) {
        // Initialize variables for loading of shader files
        std::string vertexCodeStr;
        std::string fragmentCodeStr;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;

        // Ensure ifstream objects can throw exceptions
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try {
            std::cout << "Loading Shader files..." << std::endl;
            // Open files
            vShaderFile.open(vertPath);
            fShaderFile.open(fragPath);
            std::stringstream vShaderStream, fShaderStream;
            // Read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // Close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // Convert stream into string
            vertexCodeStr = vShaderStream.str();
            fragmentCodeStr = fShaderStream.str();
        }
        // If there's an error in reading the file
        catch (std::ifstream::failure& e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
        }

        std::cout << "Loaded Shader files successfully! \n" << std::endl;

        // Convert shader file content
        const char* vertexCode = vertexCodeStr.c_str();
        const char* fragmentCode = fragmentCodeStr.c_str();

        // Create a Vertex Shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        // Assign the Vertex Shader file to the Vertex Shader
        glShaderSource(vertexShader, 1, &vertexCode, NULL);
        // Compile the Vertex Shader
        glCompileShader(vertexShader);
        // Check for compilation errors
        checkCompileErrors(vertexShader, "VERTEX");

        // Create a Fragment Shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        // Assign the Fragment Shader file to the Fragment Shader
        glShaderSource(fragmentShader, 1, &fragmentCode, NULL);
        // Compile the Fragment Shader
        glCompileShader(fragmentShader);
        // Check for compilation errors
        checkCompileErrors(vertexShader, "FRAGMENT");

        // Create the shader program
        shaderProgramID = glCreateProgram();
        glAttachShader(shaderProgramID, vertexShader);
        glAttachShader(shaderProgramID, fragmentShader);
        glLinkProgram(shaderProgramID);
        // Check for compilation errors
        checkCompileErrors(shaderProgramID, "PROGRAM");

        // Delete shaders since they are linked already
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    }

    // Use this shader.
    void use() const {
        glUseProgram(shaderProgramID);
    }

    /******** UTILITY FUNCTIONS ********/
    // Set a boolean variable value within the shader.
    void setBool(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), (int)value);
    }

    // Set an integer variable value within the shader.
    void setInt(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(shaderProgramID, name.c_str()), value);
    }

    // Set a float variable value within the shader.
    void setFloat(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(shaderProgramID, name.c_str()), value);
    }

    // Set a vec2 variable value within the shader.
    void setVec2(const std::string& name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, &value[0]);
    }

    // Set a vec2 variable value within the shader with separate x and y values.
    void setVec2(const std::string& name, float x, float y) const {
        glUniform2f(glGetUniformLocation(shaderProgramID, name.c_str()), x, y);
    }

    // Set a vec3 variable value within the shader.
    void setVec3(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, &value[0]);
    }

    // Set a vec3 variable value within the shader with separate x, y, and z values.
    void setVec3(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(shaderProgramID, name.c_str()), x, y, z);
    }

    // Set a vec4 variable value within the shader.
    void setVec4(const std::string& name, const glm::vec4& value) const {
        glUniform4fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, &value[0]);
    }

    // Set a vec4 variable value within the shader with separate x, y, z, and w values.
    void setVec4(const std::string& name, float x, float y, float z, float w) const {
        glUniform4f(glGetUniformLocation(shaderProgramID, name.c_str()), x, y, z, w);
    }

    // Set a 2x2 matrix value within the shader.
    void setMat2(const std::string& name, const glm::mat2& mat) const {
        glUniformMatrix2fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    // Set a 3x3 matrix value within the shader.
    void setMat3(const std::string& name, const glm::mat3& mat) const {
        glUniformMatrix3fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

    // Set a 4x4 matrix value within the shader.
    void setMat4(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(shaderProgramID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
};