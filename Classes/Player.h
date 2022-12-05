#pragma once

/*
	Player class implementation. Holds every player-related functionality.
 */
class Player{
	// TODO: Implement this
	/*
		- Light intensity control
		- Mouse control
		- Submarine control
		- Camera control (?)

		- Control point light intensity
		- Control 3rd person view using mouse (cannot see far)
		- Control 1st person view using keeb (can see further; single color)
		- Control ortho camera using WASD
	*/
private:
	// The model representation of the player in the screen
	Model* model;

	// 1st point of view (POV) camera; inside the player's model (sonar-like view)
	PerspectiveCamera* firstPOVCamera;
	// 3rd POV camera; rotates spherically around the player's model
	PerspectiveCamera* thirdPOVCamera;
	// Distance of third POV camera from player's model
	const float thirdPOVCameraDist = 10.0f;
	// Determines if player's POV camera is currently being used as the view or not
	bool showPlayerPOVCamera;
	// Determines if the current POV camera of player being used is first or third POV camera
	bool showFirstPOVCamera;

	// Model and first POV camera movement speed
	const float moveSpeed = 0.5f;
	// Model and first POV camera rotation speed
	const float rotSpeed = 10.0f;

	// Point light which points in front of the player's model
	PointLight* pointLight;
	// Distance of point light from player's model
	const float lightDist = 5.0f;
	// Default color of point light; change as necessary
	const glm::vec3 defaultPointLightColor = glm::vec3(1.0f);
	// Point light intensity values (low, medium, and high)
	const float lightIntensityVals[3]{0.5, 1.0, 2.0};
	// Point light intensity value index
	int lightIntensityValIndex;

	// Updates the position of the player's third POV camera based on the player model's new position
	void updateThirdPOVCameraPositionOnModel() {
		glm::vec3 thirdPOVCameraPos = this->model->getPosition();
		float yaw = this->thirdPOVCamera->getYaw();
		float pitch = this->thirdPOVCamera->getPitch();
		thirdPOVCameraPos.x *= sin(glm::radians(yaw)) * cos(glm::radians(pitch)) * this->thirdPOVCameraDist;
		thirdPOVCameraPos.y *= sin(glm::radians(pitch)) * this->thirdPOVCameraDist;
		thirdPOVCameraPos.z *= cos(glm::radians(yaw)) * cos(glm::radians(pitch)) * -this->thirdPOVCameraDist;
		this->thirdPOVCamera->setPosition(thirdPOVCameraPos);
	}

	// Updates the position of the point light based on the player model's new transformation matrix
	void updatePointLightPositionOnModel() {
		glm::vec4 oldLightPos = glm::vec4(this->pointLight->getPosition(), 1.0f);
		glm::mat4 transform = this->model->computeTransMatrix();
		glm::vec3 newLightPos = glm::vec3(transform * oldLightPos);
		newLightPos.z += this->lightDist;
		this->pointLight->setPosition(newLightPos);
	}

public:
	// Instantiates a Player object.
	Player(
		Model model,
		PerspectiveCamera firstPOVCamera, 
		PerspectiveCamera thirdPOVCamera, 
		PointLight pointLight,
		bool showPlayerPOVCamera = false, // Start of the program should use player pov camera first
		bool showFirstPOVCamera = false,  // Player's third POV camera is used first as well
		int lightIntensityValIndex = 1    // Start at medium light intensity
	) {
		// Initialize attributes
		this->model = &model;
		this->firstPOVCamera = &firstPOVCamera;
		this->thirdPOVCamera = &thirdPOVCamera;
		this->pointLight = &pointLight;
		this->showPlayerPOVCamera = showPlayerPOVCamera;
		this->showFirstPOVCamera = showFirstPOVCamera;
		this->lightIntensityValIndex = lightIntensityValIndex;
	}

	// Draws the player's elements using the shader.
	void draw(Shader shader) {
		// Bind the point light to the shader
		this->pointLight->bindToShader(shader);
		
		// Bind the perspective camera being currently used (1st or 3rd POV)
		// Only if current view is not in orthographic top view (bird's eye view)
		if (this->showPlayerPOVCamera) {
			// If first POV camera is being currently used
			if (this->showFirstPOVCamera) {
				this->firstPOVCamera->bindToShader(shader);
			}
			else {
				this->thirdPOVCamera->bindToShader(shader);
			}
		}
		
		// Draw the player's model
		this->model->draw(shader);
	}

	// Moves the player forward.
	void moveForward() {
		// Move the player's model forward
		glm::vec3 modelPos = this->model->getPosition();
		modelPos.z += this->moveSpeed;
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
	void moveBackward() {
		// Move the player's model backwards
		glm::vec3 modelPos = this->model->getPosition();
		modelPos.z -= this->moveSpeed;
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
		modelRot.x -= this->rotSpeed;
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
		modelRot.x += this->rotSpeed;
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
		// Move the player's model upwards
		glm::vec3 modelPos = this->model->getPosition();
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
		pitch += offsetY;
		yaw += offsetX;
		this->thirdPOVCamera->setPitch(pitch);
		this->thirdPOVCamera->setYaw(yaw);

		// Get new camera direction based on distance and rotation values
		glm::vec3 direction;

		// Make camera rotate left or right around the main object
		direction.x = this->thirdPOVCameraDist * sin(glm::radians(yaw)) * cos(glm::radians(pitch));

		// Make camera rotate up and down around the main object
		// Positive so that the camera will rotate downwards when the cursor goes up
		direction.y = this->thirdPOVCameraDist * sin(glm::radians(pitch));

		// Negative so that the camera will look in front of the object at start
		direction.z = -this->thirdPOVCameraDist * cos(glm::radians(yaw)) * cos(glm::radians(pitch));

		// Update the position vector only of the third POV camera
		// Camera center will not be changed so that the rotated camera will still point at the main object
		// This makes the camera rotate around the object in a spherical manner
		this->thirdPOVCamera->setPosition(direction);
	}

	// Changes (cycles through) the intensity of the model's point light.
	void changeLightIntensity() {
		// If previous light intensity value was high, move back to low light intensity value
		if (this->lightIntensityValIndex == 2) {
			this->lightIntensityValIndex = 0;
		}
		else {
			// Increment light intensity value index
			this->lightIntensityValIndex++;
		}

		// Change light intensity
		this->pointLight->setLightColor(
			this->defaultPointLightColor * this->lightIntensityVals[this->lightIntensityValIndex]
		);
	}

	// Toggles the player's POV camera; if it is to be used or not.
	void toggleCamera() {
		this->showPlayerPOVCamera = !this->showPlayerPOVCamera;
	}

	// Returns the boolean value indicating if player's POV camera is currently being used or not.
	bool isPOVCameraUsed() {
		return this->showPlayerPOVCamera;
	}

	// Changes the player's POV camera to be used; if first or third POV.
	void changeCamera() {
		this->showFirstPOVCamera = !this->showFirstPOVCamera;
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

