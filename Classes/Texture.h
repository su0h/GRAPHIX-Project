#pragma once

/*
    Texture class implementation. Holds values for texture-related functionality.
 */
class Texture {
private:
    // Unique ID of the texture (generated with glGenTextures())
    GLuint textureID;
    // Texture unit used by the texture (i.e., GL_TEXTURE0)
    GLuint textureUnit;

public:
    // Instantiates a Texture object.
    Texture(
        GLuint textureID = 0,
        GLuint textureUnit = GL_TEXTURE0
    ) {
        this->textureID = textureID;
        this->textureUnit = textureUnit;
    }

    // Binds this Texture object to the current shader in use.
    void bind() {
        glActiveTexture(textureUnit);
        glBindTexture(GL_TEXTURE_2D, textureID);
    }

    // Returns the unique ID of this texture.
    GLuint getTextureID() {
        return this->textureID;
    }

    // Returns the texture unit this texture is affiliated with.
    GLuint getTextureUnit() {
        return this->textureUnit;
    }
};