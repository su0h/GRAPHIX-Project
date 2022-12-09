#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/* 
	Abstract Camera class implementation. Parent class for every camera-related functionality.
 */
class Camera {
protected:
	// Position of the camera
	glm::vec3 position;
	// WorldUp of the camera
	glm::vec3 worldUp;
	// Where the camera points at
	glm::vec3 center;
	// Yaw value of the camera
	float yaw;
	// Pitch value of the camera
	float pitch;
	// zNear value of the camera
	float zNear;
	// zFar value of the camera
	float zFar;

	// Update the camera's center.
	void updateCamera() {
		this->center = glm::normalize(
			glm::vec3(
				cos(glm::radians(yaw)) * cos(glm::radians(pitch)),
				sin(glm::radians(pitch)),
				sin(glm::radians(yaw)) * cos(glm::radians(pitch))
			)
		);
	}

public:
	// Bind the attributes of this camera to the specified shader. To be implemented.
	virtual void bindToShader(Shader shader, bool isSkybox = false) = 0;

	// Compute for the projection matrix of this camera.
	virtual glm::mat4 computeProjectionMatrix() = 0;

	// Compute for the view matrix and return it.
	glm::mat4 computeViewMatrix() {
		return glm::lookAt(this->position, this->center, this->worldUp);
	}

	// Sets the position of the camera.
	void setPosition(glm::vec3 position) {
		this->position = position;
	}

	// Sets the center of the camera.
	void setCenter(glm::vec3 center) {
		this->center = center;
	}

	// Sets the center of the camera based on new pitch and yaw values
	void setCenter(float pitch, float yaw) {
		this->pitch = pitch;
		this->yaw = yaw;
		updateCamera();
	}

	// Sets the world up of the camera.
	void setWorldUp(glm::vec3 worldUp) {
		this->worldUp = worldUp;
	}

	// Returns the position of the camera.
	glm::vec3 getPosition() {
		return glm::vec3(this->position);
	}

	// Returns the center vector of the camera.
	glm::vec3 getCenter() {
		return glm::vec3(this->center);
	}

	// Returns the WorldUp vector of the camera.
	glm::vec3 getWorldUp() {
		return glm::vec3(this->worldUp);
	}

	// Returns the yaw value of the camera.
	float getYaw() {
		return this->yaw;
	}

	// Sets the new yaw value of the camera.
	void setYaw(float yaw) {
		this->yaw = yaw;
	}

	// Returns the pitch value of the camera.
	float getPitch() {
		return this->pitch;
	}

	// Sets the new pitch value of the camera
	void setPitch(float pitch) {
		this->pitch = pitch;
	}

	// Returns the zNear value of the camera.
	float getZNear() {
		return this->zNear;
	}

	// Returns the zFar value of the camera.
	float getZFar() {
		return this->zFar;
	}
};

/*
	Orthographic Camera class implementation. Holds every Orthographic Camera-related functionalities. Inherits members from the abstract Camera class.
 */
class OrthoCamera : public Camera {
private:
	// Leftmost point to be included in the view
	float left;
	// Rightmost point to be included in the view
	float right;
	// Bottommost point to be included in the view
	float bottom;
	// Topmost point to be included in the view
	float top;

public:
	// Initializes an Orthographic Camera object.
	OrthoCamera(
		glm::vec3 position,
		glm::vec3 worldUp,
		glm::vec3 center,
		float left,
		float right,
		float bottom,
		float top,
		float zNear,
		float zFar,
		float yaw = 0.0f,
		float pitch = 0.0f
	) {
		// Initialize attributes
		this->position = position;
		this->worldUp = worldUp;
		this->center = center;
		this->yaw = yaw;
		this->pitch = pitch;
		this->left = left;
		this->right = right;
		this->bottom = bottom;
		this->top = top;
		this->zNear = zNear;
		this->zFar = zFar;

		// Initialize camera center
		updateCamera();
	}

	// Virtual function implementation. Bind the attributes of this Orthographic Camera to the specified shader.
	void bindToShader(Shader shader, bool isSkybox = false) {
		shader.setVec3("cameraPos", this->position);
		shader.setMat4("projection", this->computeProjectionMatrix());

		// If shader is intended for a skybox, modify view matrix
		if (isSkybox) {
			glm::mat4 sky_view = glm::mat4(1.0f);
			sky_view = glm::mat4(glm::mat3(this->computeViewMatrix()));
			shader.setMat4("view", sky_view);
		}
		// Else, just use the usual computation
		else {
			shader.setMat4("view", this->computeViewMatrix());
		}
	}

	// Virtual function implementation. Compute for the projection matrix of this camera.
	glm::mat4 computeProjectionMatrix() {
		return glm::ortho(
			this->left,
			this->right,
			this->bottom,
			this->top,
			this->zNear,
			this->zFar
		);
	}
};

/*
	Perspective Camera class implementation. Holds every Perspective Camera-related functionalities. Inherits members from the abstract Camera class.
 */
class PerspectiveCamera : public Camera {
private:
	// Field of view of the camera
	float fieldOfView;
	// Aspect ratio of the camera
	float aspectRatio;

public:
	// Initializes a Perspective Camera object.
	PerspectiveCamera(
		glm::vec3 position,
		glm::vec3 worldUp,
		glm::vec3 center,
		float fieldOfView,
		float aspectRatio,
		float zNear,
		float zFar,
		float yaw = 0.0f,
		float pitch = 0.0f
	) {
		// Initialize attributes
		this->position = position;
		this->worldUp = worldUp;
		this->center = center;
		this->fieldOfView = fieldOfView;
		this->aspectRatio = aspectRatio;
		this->zNear = zNear;
		this->zFar = zFar;
		this->yaw = yaw;
		this->pitch = pitch;

		// Initialize camera center
		updateCamera();
	}

	// Virtual function implementation. Binds the attributes of this Perspective Camera to the specified shader.
	void bindToShader(Shader shader, bool isSkybox = false) {
		shader.setVec3("cameraPos", this->position);
		shader.setMat4("projection", this->computeProjectionMatrix());

		// If shader is intended for a skybox, modify view matrix
		if (isSkybox) {
			glm::mat4 sky_view = glm::mat4(1.0f);
			sky_view = glm::mat4(glm::mat3(this->computeViewMatrix()));
			shader.setMat4("view", sky_view);
		}
		// Else, just use the usual computation
		else {
			shader.setMat4("view", this->computeViewMatrix());
		}
	}

	// Virtual function implementation. Compute for the projection matrix of this camera.
	glm::mat4 computeProjectionMatrix() {
		return glm::perspective(
			this->fieldOfView,
			this->aspectRatio,
			this->zNear,
			this->zFar
		);
	}

	// Bind the attributes of this camera based on first person POV movement.
	void bindToShaderFirstPOV(Shader shader, bool isSkybox = false) {
		shader.setVec3("cameraPos", this->position);
		shader.setMat4("projection", this->computeProjectionMatrix());

		// If shader is intended for a skybox, modify view matrix
		if (isSkybox) {
			glm::mat4 sky_view = glm::mat4(1.0f);
			sky_view = glm::mat4(glm::mat3(this->computeViewMatrixFirstPOV()));
			shader.setMat4("view", sky_view);
		}
		// Else, just use the usual computation
		else {
			shader.setMat4("view", this->computeViewMatrixFirstPOV());
		}
	}

	// Computes the view matrix of this camera based on first person POV movement.
	glm::mat4 computeViewMatrixFirstPOV() {
		return glm::lookAt(this->position, this->position + this->center, this->worldUp);
	}
};