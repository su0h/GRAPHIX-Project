#pragma once

/*
	Player class implementation. Holds every player-related functionality.
 */
class Player{
private:
	// The model representation of the player in the screen
	Model* model;

	// 1st point of view (POV) camera; inside the player's model (sonar-like view)
	PerspectiveCamera* firstPOVCamera;
	// 3rd POV camera; rotates spherically around the player's model
	PerspectiveCamera* thirdPOVCamera;
	// Distance of third POV camera from player's model
	const float thirdPOVCameraDist = 30.0f;
	// Determines if player's POV camera is currently being used as the view or not
	bool showPlayerPOVCamera;
	// Determines if the current POV camera of player being used is first or third POV camera
	bool showFirstPOVCamera;

	// Model and first POV camera movement speed
	const float moveSpeed = 0.5f;
	// Model and first POV camera rotation speed
	const float rotSpeed = 0.5f;

	// Point light which points in front of the player's model
	PointLight* pointLight;
	// Distance of point light from player's model
	const float lightDist = 15.0f;
	// Default color of point light; change as necessary
	const glm::vec3 defaultPointLightColor = glm::vec3(1.0f);
	// Point light intensity values (low, medium, and high)
	const float lightIntensityVals[3]{1.5, 2.5, 3.5};
	// Point light intensity value index
	int lightIntensityValIndex;

	// Updates the position of the player's third POV camera based on the player model's new position
	void updateThirdPOVCameraPositionOnModel() {
		// Get position and rotation vectors of player's model
		glm::vec3 modelPos = this->model->getPosition();
		glm::vec3 modelRot = this->model->getRotation();

		// Get current pitch and yaw values of third POV camera
		float pitch = this->thirdPOVCamera->getPitch();
		float yaw = this->thirdPOVCamera->getYaw();

		// Update position of third POV camera based on new position and rotation of player's model
		// Should be n distance units away from the player's model
		glm::vec3 cameraPos = this->thirdPOVCamera->getPosition();
		cameraPos.x = modelPos.x + sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * this->thirdPOVCameraDist;
		cameraPos.y = modelPos.y + sin(glm::radians(pitch)) * this->thirdPOVCameraDist;
		cameraPos.z = modelPos.z + cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * -this->thirdPOVCameraDist;

		// Set new position of third POV camera
		this->thirdPOVCamera->setPosition(cameraPos);

		// Make third POV camera point to model's new position
		this->thirdPOVCamera->setCenter(modelPos);
	}

	// Updates the position of the point light based on the player model's new transformation matrix
	void updatePointLightPositionOnModel() {
		// Get position of player's model
		glm::vec3 modelPos = this->model->getPosition();

		// Get y-axis rotation value of player's model
		float modelRotY = this->model->getRotation().y;

		// Calculate new position of light based on current position and y-axis rotation of model
		modelPos.x += sin(glm::radians(modelRotY)) * this->lightDist;
		modelPos.z += cos(glm::radians(modelRotY)) * this->lightDist;

		// Set new position of light
		this->pointLight->setPosition(modelPos);
	}

public:
	// Instantiates a Player object.
	Player(
		Model* model,
		PerspectiveCamera* firstPOVCamera, 
		PerspectiveCamera* thirdPOVCamera, 
		PointLight* pointLight,
		bool showPlayerPOVCamera = true,  // Start of the program should use player pov camera first
		bool showFirstPOVCamera = false,  // Player's third POV camera is used first as well
		int lightIntensityValIndex = 0    // Start at low light intensity
	) {
		// Initialize attributes
		this->model = model;
		this->firstPOVCamera = firstPOVCamera;
		this->thirdPOVCamera = thirdPOVCamera;
		this->pointLight = pointLight;
		this->showPlayerPOVCamera = showPlayerPOVCamera;
		this->showFirstPOVCamera = showFirstPOVCamera;
		this->lightIntensityValIndex = lightIntensityValIndex;

		// Update position of point light for it to be facing in front of the model.
		updatePointLightPositionOnModel();

		// Set intensity of light to initial light intensity value
		this->pointLight->setLightColor(
			this->defaultPointLightColor * this->lightIntensityVals[this->lightIntensityValIndex]
		);
	}

	// Draws the player's elements using the shader.
	void draw(Shader shader) {
		// Bind the perspective camera being currently used (1st or 3rd POV)
		// Only if current view is not in orthographic top view (bird's eye view)
		if (this->showPlayerPOVCamera) {
			// If first POV camera is being currently used
			if (this->showFirstPOVCamera) {
				this->firstPOVCamera->bindToShaderFirstPOV(shader, false);
			}
			else {
				this->thirdPOVCamera->bindToShader(shader);
			}
		}

		// Bind the point light to the shader
		this->pointLight->bindToShader(shader);
		
		// Draw the player's model if current camera view is not first POV or is top view
		if (!this->showFirstPOVCamera || !this->showPlayerPOVCamera) {
			this->model->draw(shader);
		}
	}

	// Moves the player forward.
	void moveForward() {
		// Move the player's model forward
		glm::vec3 modelPos = this->model->getPosition();
		glm::vec3 modelRot = this->model->getRotation();
		modelPos.x += -sin(glm::radians(modelRot.y)) * -this->moveSpeed;
		modelPos.z += -cos(glm::radians(modelRot.y)) * -this->moveSpeed;
		this->model->setPosition(modelPos);

		// Move the first POV camera forward
		glm::vec3 firstPOVCameraPos = this->firstPOVCamera->getPosition();
		glm::vec3 firstPOVCameraCenter = this->firstPOVCamera->getCenter();
		firstPOVCameraPos += firstPOVCameraCenter * this->moveSpeed;
		this->firstPOVCamera->setPosition(firstPOVCameraPos);

		// Update the position of the third POV camera
		updateThirdPOVCameraPositionOnModel();

		// Update the position of the point light
		updatePointLightPositionOnModel();
	}

	// Moves the player backwards.
	void moveBackwards() {
		// Move the player's model backwards
		glm::vec3 modelPos = this->model->getPosition();
		glm::vec3 modelRot = this->model->getRotation();
		modelPos.x += sin(glm::radians(modelRot.y)) * -this->moveSpeed;
		modelPos.z += cos(glm::radians(modelRot.y)) * -this->moveSpeed;
		this->model->setPosition(modelPos);

		// Move the first POV camera backwards
		glm::vec3 firstPOVCameraPos = this->firstPOVCamera->getPosition();
		glm::vec3 firstPOVCameraCenter = this->firstPOVCamera->getCenter();
		firstPOVCameraPos -= firstPOVCameraCenter * this->moveSpeed;
		this->firstPOVCamera->setPosition(firstPOVCameraPos);

		// Update the position of the third POV camera
		updateThirdPOVCameraPositionOnModel();

		// Update the position of the point light
		updatePointLightPositionOnModel();
	}

	// Rotates the player to the left.
	void turnLeft() {
		// Rotate the player's model to the left
		glm::vec3 modelRot = this->model->getRotation();
		modelRot.y += this->rotSpeed;
		this->model->setRotation(modelRot);

		// Rotate the first POV camera to the left
		float yaw = this->firstPOVCamera->getYaw() - this->rotSpeed;
		float pitch = this->firstPOVCamera->getPitch();
		this->firstPOVCamera->setCenter(pitch, yaw);

		// Update the position of the point light
		updatePointLightPositionOnModel();
	}

	// Rotates the player to the right.
	void turnRight() {
		// Rotate the player's model to the right
		glm::vec3 modelRot = this->model->getRotation();
		modelRot.y -= this->rotSpeed;
		this->model->setRotation(modelRot);

		// Rotate the first POV camera to the right
		float yaw = this->firstPOVCamera->getYaw() + this->rotSpeed;
		float pitch = this->firstPOVCamera->getPitch();
		this->firstPOVCamera->setCenter(pitch, yaw);

		// Update the position of the point light
		updatePointLightPositionOnModel();
	}

	// Ascends the player.
	void ascend() {
		// Get position of player's model
		glm::vec3 modelPos = this->model->getPosition();

		// Only ascend if current y-coordinate position is below 0.
		if (modelPos.y < 0) {
			// Move the player's model upwards
			modelPos.y += this->moveSpeed;
			this->model->setPosition(modelPos);

			// Move the first POV camera upwards
			glm::vec3 firstPOVCameraPos = this->firstPOVCamera->getPosition();
			glm::vec3 firstPOVCameraUp = this->firstPOVCamera->getWorldUp();
			firstPOVCameraPos += firstPOVCameraUp * this->moveSpeed;
			this->firstPOVCamera->setPosition(firstPOVCameraPos);

			// Update the position of the third POV camera
			updateThirdPOVCameraPositionOnModel();

			// Update the position of the point light
			updatePointLightPositionOnModel();
		}
	}

	// Descends the player.
	void descend() {
		// Move the player's model downwards
		glm::vec3 modelPos = this->model->getPosition();
		modelPos.y -= this->moveSpeed;
		this->model->setPosition(modelPos);

		// Move the first POV camera downwards
		glm::vec3 firstPOVCameraPos = this->firstPOVCamera->getPosition();
		glm::vec3 firstPOVCameraUp = this->firstPOVCamera->getWorldUp();
		firstPOVCameraPos -= firstPOVCameraUp * this->moveSpeed;
		this->firstPOVCamera->setPosition(firstPOVCameraPos);

		// Update the position of the third POV camera
		updateThirdPOVCameraPositionOnModel();

		// Update the position of the point light 
		updatePointLightPositionOnModel();
	}

	// Rotates the player's third POV camera based on mouse inputs.
	void rotateThirdPOVCameraOnMouse(float offsetX, float offsetY) {
		// Update the pitch and yaw values of the third POV camera
		float pitch = this->thirdPOVCamera->getPitch();
		float yaw = this->thirdPOVCamera->getYaw();
		pitch += offsetY; // rotate 3rd pov camera up or down
		yaw += offsetX;   // rotate 3rd pov camera left or right
		this->thirdPOVCamera->setPitch(pitch);
		this->thirdPOVCamera->setYaw(yaw);

		// Get new camera position based on distance and rotation values
		glm::vec3 newPos;

		// Make camera rotate left or right around the main object
		newPos.x = this->thirdPOVCameraDist * sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		// Make camera rotate up and down around the main object
		// Positive so that the camera will rotate downwards when the cursor goes up
		newPos.y = this->thirdPOVCameraDist * sin(glm::radians(pitch));

		// Negative so that the camera will look in front of the object at start
		newPos.z = -this->thirdPOVCameraDist * cos(glm::radians(yaw)) * cos(glm::radians(pitch));

		// Get position of player's model
		glm::vec3 modelPos = this->model->getPosition();

		// Adjust position of third POV camera based on current position of player's model since it moves
		// In case the model changed its position recently
		newPos += modelPos;

		// Update the position vector of the third POV camera
		this->thirdPOVCamera->setPosition(newPos);

		// Point at position of model; in case it was changed recently
		this->thirdPOVCamera->setCenter(modelPos);
	}

	// Changes (cycles through) the intensity of the model's point light.
	void changeLightIntensity() {
		// Increment light intensity value index
		this->lightIntensityValIndex++;

		// If previous light intensity value was high, move back to low light intensity value
		if (this->lightIntensityValIndex == 3) {
			this->lightIntensityValIndex = 0;
		}

		// Change light intensity
		this->pointLight->setLightColor(
			this->defaultPointLightColor * this->lightIntensityVals[this->lightIntensityValIndex]
		);
	}

	// Toggles the player's POV camera; if it is to be used or not.
	void toggleCamera(bool use) {
		this->showPlayerPOVCamera = use;
	}

	// Returns the boolean value indicating if player's POV camera is currently being used or not.
	bool isPOVCameraUsed() {
		return this->showPlayerPOVCamera;
	}

	// Toggles the player's first POV camera; if using first POV or not.
	void toggleFirstPOVCamera(bool use) {
		this->showFirstPOVCamera = use;
	}

	// Returns the boolean value indicating if the current POV camera of player being used is first POV camera.
	bool isFirstPOVCameraUsed() {
		return this->showFirstPOVCamera;
	}

	// Returns the player's model.
	Model* getModel() {
		return this->model;
	}

	// Returns the first POV camera of the player.
	PerspectiveCamera* getFirstPOVCamera() {
		return this->firstPOVCamera;
	}
	
	// Returns the third POV camera of the player.
	PerspectiveCamera* getThirdPOVCamera() {
		return this->thirdPOVCamera;
	}

	// Returns the point light of the player's model.
	PointLight* getPointLight() {
		return this->pointLight;
	}
};

