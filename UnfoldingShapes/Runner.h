#ifndef RUNNER_H
#define RUNNER_H

#include <qobject.h>
#include <qtimer.h>

#include "OpenGLWidget.h"
#include "UnfoldingShapes.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <vector>
#include <chrono>
#include <cmath>
#include <ctime>
#include <windows.h>

// graphics tools
#include "Camera.h"
#include "Light.h"
#include "Asset.h"
#include "Model.h"
#include "Mesh.h"

// Unfold tools
#include "Shape.h"
#include "UnfoldSolution.h"
#include "Unfold.h"
#include "Animator.h"

using namespace std;

class Runner : public QObject {
public:
	// resources
	const char* cubeModel = "resources\\objects\\cube\\cube.obj";
	const char* dodecahedronModel = "resources\\objects\\dodecahedron\\dodecahedron.obj";
	const char* ballModel = "resources\\objects\\redball\\redball.obj";
	const char* boardModel = "resources\\objects\\3dmill\\3dmill.obj";
	const char* backpackModel = "resources\\objects\\testing\\backpack\\backpack.obj";
	const char* humanoidModel = "resources\\objects\\simplehumanoid\\simplehumanoid.obj";

	// legacy
	const char* fourModel = "resources\\objects\\3dfourconnect\\3dfourconnectFIXED.obj";

	// skybox paths
	const std::vector<const char*> cloudySkybox
	{
		"resources/textures/CloudySkyBox/cubemap_1.jpg",
		"resources/textures/CloudySkyBox/cubemap_3.jpg",
		"resources/textures/CloudySkyBox/cubemap_4.jpg",
		"resources/textures/CloudySkyBox/cubemap_5.jpg",
		"resources/textures/CloudySkyBox/cubemap_0.jpg",
		"resources/textures/CloudySkyBox/cubemap_2.jpg"
	};

	// prototypes
	// control callback for clicking the mouse
	// void mouse_button_callback_custom(GLFWwindow* window, int button, int action, int mods);
	// control callback for moving the mouse
	// void mouse_callback_custom(GLFWwindow* window, double xpos, double ypos);

	// void updateControls(GLFWwindow* window, Animator &animator);

	// settings
	float relativeScreenSize = 0.85;

	float aspectRatio = 16.0f / 9.0f;

	int samples = 16;

	// default
	unsigned int SCR_WIDTH = 1600 * relativeScreenSize;
	unsigned int SCR_HEIGHT = 900 * relativeScreenSize;

	// fps info
	bool fpsCounterEnabled = true;
	const double fps = 60;

	int fpsCount;
	int fpsCounter;

	// game
	int gameState;

	OpenGLWidget* graphics;
	UnfoldingShapes* ui;

	vector<Shape*>* shapes;

	Animator animator;

	QTimer *timer;

	Runner(OpenGLWidget* graphics, UnfoldingShapes* ui, QObject* parent = nullptr) : QObject(parent) {
		this->graphics = graphics;
		this->ui = ui;

		setup();
	}

	void setup() {
		// set timer for each frame to update
		timer = new QTimer(this);
		QObject::connect(timer, &QTimer::timeout, this, &Runner::frame);
		timer->start(1000/fps);

		// add all the models that are going to be used immediatley

		// set skybox
		// graphics->setSkybox(cloudySkybox);

		// set light
		graphics->setLight(glm::vec3(100, 150, 100), glm::vec3(1));
		graphics->getLight()->visible = false;


		// set camera starting pos
		graphics->camera.setPos(glm::vec3(10.0f, 5.0f, -10.0f) * 0.33f * 2.0f);
		graphics->camera.yaw += 45.0f;
		graphics->camera.pitch -= 22.5f;

		// set text
		//graphics->addText("FPS: 0", "fps", 1, 95, 0.5f, glm::vec3(1.0, 0.0, 0.0));
		//graphics->addText("Position: " + glm::to_string(graphics->camera.pos), "position", 63, 95, 0.5f, glm::vec3(1.0, 0.0, 0.0));


		// set callbacks
		//glfwSetCursorPosCallback(graphics->window, mouse_callback_custom);
		//glfwSetMouseButtonCallback(graphics->window, mouse_button_callback_custom);

		// setup shapes
		shapes = new vector<Shape*>;
		animator = Animator();

		// set links
		ui->linkAnimator(&animator);
		ui->linkShapes(shapes);

		// add assets
		//shapes.push_back(new Shape(backpackModel));
		//shapes.push_back(new Shape(humanoidModel));
		//shapes.push_back(new Shape(ballModel));
		addShape(new Shape(cubeModel, graphics));
		//shapes.push_back(new Shape(dodecahedronModel, graphics));

		// fps and game init
		fpsCount = 0;
		fpsCounter = 0;

		gameState = 1;
	}

	void frame() {
		//std::cout << "here" << std::endl;

		// START timer
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

		// Main
		// update controls
		//updateControls(graphics->window, animator);

		animator.update();

		// update player position
		//graphics->setText("position", "Position: " + glm::to_string(graphics->camera.pos));

		// update renderer
		graphics->update();

		// END of timer sleep and normalize the clock
		std::chrono::system_clock::time_point after = std::chrono::system_clock::now();
		std::chrono::microseconds difference(std::chrono::time_point_cast<std::chrono::microseconds>(after) - std::chrono::time_point_cast<std::chrono::microseconds>(now));

		// count the fps
		int diffCount = difference.count();
		if (diffCount == 0) {
			diffCount = 1;
		}

		int sleepDuration = ((1000000 / fps * 1000) - diffCount) / 1000000;

		// output fps
		fpsCount += 1;
		fpsCounter += 1000000 / diffCount;

		if (fpsCount % int(fps) == 0) {
			if (fpsCounterEnabled) {
				std::cout << "\rFPS: " << fpsCounter / fpsCount;

				// set text
				//graphics->setText("fps", "FPS: " + std::to_string(int(fpsCounter / fpsCount)));
			}
			fpsCount = 0;
			fpsCounter = 0;
		}

		if (sleepDuration < 0) {
			sleepDuration = 0;
		}

		// std::cout << sleepDuration << std::endl;
		Sleep(sleepDuration);
	}

	// add shape to animator
	void addShape(Shape* shape) {
		shapes->push_back(shape);
		graphics->addAsset((*shapes)[shapes->size() - 1]->asset);

		ui->addShapeToList((*shapes)[shapes->size() - 1]);
	}

	// utility functions
	// booleans to help make sure holding down a button does not spam something
	bool pressedEnter = false;
	bool pressedTab = false;

	/*
	void updateControls(GLFWwindow* window, Animator &animator) {
		// pause or play (pressed Enter ensures that if the user holds the button then it won't try to play and pause over and over really fast)
		if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
			if (!pressedEnter) {
				pressedEnter = true;

				if (animator.isPaused()) {
					animator.play();
				}
				else {
					animator.pause();
				}
			}
		}
		else {
			pressedEnter = false;
		}

		// increase speed
		if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
			animator.incrementSpeed(0.01f);
		}

		// decrease speed
		if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
			animator.incrementSpeed(-0.01f);
		}

		// scrubbing
		// right - forward
		if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
			animator.scrub(0.0025);
			animator.pause();
		}

		// left - backward
		if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
			animator.scrub(-0.0025);
			animator.pause();
		}

		// Tab - Switch Algorithm
		if (glfwGetKey(window, GLFW_KEY_TAB) == GLFW_PRESS) {
			if (!pressedTab) {
				pressedTab = true;

				animator.shuffleAlgorithm();
			}
		}
		else {
			pressedTab = false;
		}
	}
	*/
};
#endif