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
#include "Skybox.h"
#include "TextManager.h"
#include "GraphicsEngine.h"

// Unfold tools
#include "Shape.h"
#include "UnfoldSolution.h"
#include "Unfold.h"
#include "Animator.h"

using namespace std;

GraphicsEngine* graphics;

const char* cubeModel = "resources\\objects\\cube\\cube.obj";
const char* ballModel = "resources\\objects\\redball\\redball.obj";
const char* boardModel = "resources\\objects\\3dmill\\3dmill.obj";
const char* backpackModel = "resources\\objects\\testing\\backpack\\backpack.obj";
const char* humanoidModel = "resources\\objects\\simplehumanoid\\simplehumanoid.obj";

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
void mouse_button_callback_custom(GLFWwindow* window, int button, int action, int mods);
// control callback for moving the mouse
void mouse_callback_custom(GLFWwindow* window, double xpos, double ypos);

// settings
float relativeScreenSize = 0.85;

float aspectRatio = 16 / 9;

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

vector<Shape*> shapes;

Animator animator;

void setup() {
	// set window size to max while also maintaining size ratio
	RECT rect;
	GetClientRect(GetDesktopWindow(), &rect);

	SCR_WIDTH = (rect.right - rect.left) * relativeScreenSize;
	SCR_HEIGHT = (rect.bottom - rect.top) * relativeScreenSize;

	if (SCR_WIDTH / SCR_HEIGHT < aspectRatio) {
		// base the size off the width
		SCR_HEIGHT = SCR_WIDTH * (1 / aspectRatio);
	}
	if (SCR_HEIGHT / SCR_WIDTH > aspectRatio) {
		// base the size off the height
		SCR_WIDTH = SCR_HEIGHT * (aspectRatio);
	}

	// make the graphics engine (Jordan: Do not focus too much on this, it is very complicated and not relevant to the problem.
	graphics = new GraphicsEngine("3D Mill", &SCR_WIDTH, &SCR_HEIGHT, 1);

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

	// set callbacks
	//glfwSetCursorPosCallback(graphics->window, mouse_callback_custom);
	//glfwSetMouseButtonCallback(graphics->window, mouse_button_callback_custom);

	// add assets
	//shapes.push_back(new Shape(backpackModel));
	//shapes.push_back(new Shape(humanoidModel));
	shapes.push_back(new Shape(ballModel));
	//shapes.push_back(new Shape(cubeModel));

	for (int i = 0; i < shapes.size(); i++) {
		graphics->addAsset(shapes[i]->asset);
	}

	animator = Animator();
	
	// unfold = Unfold::breadthUnfold(shapes[0]);
	for (int i = 0; i < shapes.size(); i++) {
		shapes[i]->unfolds.push_back(Unfold::basic(shapes[0]));
		animator.addAnimation(shapes[i], &shapes[i]->unfolds[0], 15);
	}

	// fps and game init
	fpsCount = 0;
	fpsCounter = 0;

	gameState = 1;

	// testing
	/*
	Face::Axis axis = Face::Axis(glm::vec3(0,0,0), glm::vec3(0,1,0));
	Face::Axis axis2 = Face::Axis(glm::vec3(1, 0, 0), glm::vec3(1, 0, 1));

	std::cout << glm::to_string(axis2.line) << glm::to_string(axis2.point) << std::endl;

	axis2.rotateAxisAbout(&axis, 3.14159f * (2 / 4.0f));

	std::cout << glm::to_string(axis2.line) << glm::to_string(axis2.point) << std::endl;
	*/
}

int main() {
	setup();

	// run
	while (gameState == 1) {
		// START timer
		std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

		// Main
		animator.update();

		// render frame
		gameState = graphics->renderFrame();

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

	std::cout << "Closing" << std::endl;
	return gameState;
}