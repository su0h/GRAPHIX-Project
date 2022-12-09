#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

/*
	Abstract Light class implementation. Parent class for every light-related functionality.
 */
class Light {
protected:
	// Position of the light
	glm::vec3 position;
	// Color of the light
	glm::vec3 lightColor;
	// Color of the ambient light
	glm::vec3 ambientColor;
	// Strength of the ambient light
	float ambientStr;
	// Specular strength
	float specularStr;
	// Specular phong
	float specularPhong;

public:
	// Virtual function for the binding of light attributes to the shader.
	virtual void bindToShader(Shader shader) = 0;

	// Returns the light color of this light.
	glm::vec3 getLightColor() {
		return glm::vec3(this->lightColor);
	}

	// Returns the ambient color of this light.
	glm::vec3 getAmbientColor() {
		return glm::vec3(this->ambientColor);
	}

	// Returns the ambient strength of this light.
	float getAmbientStr() {
		return this->ambientStr;
	}

	// Returns the specular strength of this light.
	float getSpecularStr() {
		return this->specularStr;
	}

	// Returns the specular phong of this light.
	float getSpecularPhong() {
		return this->specularPhong;
	}

	// Returns the position of this light.
	glm::vec3 getPosition() {
		return glm::vec3(this->position);
	}

	// Sets the light color of this light.
	void setLightColor(glm::vec3 lightColor) {
		this->lightColor = glm::vec3(lightColor);
	}

	// Set the ambient color of this light.
	void setAmbientColor(glm::vec3 ambientColor) {
		this->ambientColor = glm::vec3(ambientColor);
	}

	// Set the ambient strength of this light.
	void setAmbientStr(float ambientStr) {
		this->ambientStr = ambientStr;
	}

	// Set the specular strength of this light.
	void setSpecularStr(float specularStr) {
		this->specularStr = specularStr;
	}

	// Set the specular phong of this light.
	void setSpecularPhong(float specularPhong) {
		this->specularPhong = specularPhong;
	}

	// Set the position of this light.
	void setPosition(glm::vec3 position) {
		this->position = glm::vec3(position);
	}
};

/*
	Point Light class implementation. Holds every Point Light-related functionalities. Inherits members from the abstract Light class.
 */
class PointLight : public Light {
private:
	// Linear value of the point light
	float linear;
	// Quadratic value of the point light
	float quadratic;

public:
	// Initializes a Point Light object.
	PointLight(
		glm::vec3 position,
		glm::vec3 lightColor,
		glm::vec3 ambientColor,
		float ambientStr,
		float specularStr,
		float specularPhong,
		float linear = 0.014f,
		float quadratic = 0.0007f
	) {
		// Initialize attributes
		this->position = glm::vec3(position);
		this->lightColor = glm::vec3(lightColor);
		this->ambientColor = glm::vec3(ambientColor);
		this->ambientStr = ambientStr;
		this->specularStr = specularStr;
		this->specularPhong = specularPhong;
		this->linear = linear;
		this->quadratic = quadratic;
	}

	// Virtual function implementation. Binds the Point Light attributes to the specified shader.
	void bindToShader(Shader shader) {
		shader.setVec3("pointLight.position", this->position);
		shader.setVec3("pointLight.lightColor", this->lightColor);
		shader.setFloat("pointLight.ambientStr", this->ambientStr);
		shader.setVec3("pointLight.ambientColor", this->ambientColor);
		shader.setFloat("pointLight.specularStr", this->specularStr);
		shader.setFloat("pointLight.specularPhong", this->specularPhong);
		shader.setFloat("pointLight.linear", this->linear);
		shader.setFloat("pointLight.quadratic", this->quadratic);
	}

	// Returns the linear value of this point light.
	float getLinear() {
		return this->linear;
	}

	// Returns the quadratic value of this point light.
	float getQuadratic() {
		return this->quadratic;
	}

	// Set the linear value of this point light.
	void setLinear(float linear) {
		this->linear = linear;
	}

	// Set the qudratic value of this point light.
	void setQuadratic(float quadratic) {
		this->quadratic = quadratic;
	}
};

/*
	Directional Light class implementation. Holds every Directional Light-related functionalities. Inherits members from the abstract Light class.
 */
class DirectionalLight : public Light {
public:
	// Initializes a Directional Light object.
	DirectionalLight(
		glm::vec3 position,
		glm::vec3 lightColor,
		glm::vec3 ambientColor,
		float ambientStr,
		float specularStr,
		float specularPhong
	) {
		// Initialize attributes
		this->position = glm::vec3(position);
		this->lightColor = glm::vec3(lightColor);
		this->ambientColor = glm::vec3(ambientColor);
		this->ambientStr = ambientStr;
		this->specularStr = specularStr;
		this->specularPhong = specularPhong;
	}

	// Virtual function implementation. Binds the Directional Light attributes to the specified shader.
	void bindToShader(Shader shader) {
		shader.setVec3("directionalLight.position", this->position);
		shader.setVec3("directionalLight.lightColor", this->lightColor);
		shader.setFloat("directionalLight.ambientStr", this->ambientStr);
		shader.setVec3("directionalLight.ambientColor", this->ambientColor);
		shader.setFloat("directionalLight.specularStr", this->specularStr);
		shader.setFloat("directionalLight.specularPhong", this->specularPhong);
	}
};