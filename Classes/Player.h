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
	Model* model;
	PerspectiveCamera* perspectiveCamera;
	OrthoCamera* orthographicCamera;
	PointLight* pointLight;
public:
	Player(
		Model model,
		PerspectiveCamera &perspectiveCamera, 
		OrthoCamera &orthographicCamera, 
		PointLight &pointLight
	) {
		this->model = &model;
		this->perspectiveCamera = &perspectiveCamera;
		this->pointLight = &pointLight;
		this->orthographicCamera = &orthographicCamera;
	}
};

