#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include <QtWidgets/qopenglwidget.h>
#include <QtGui/qopenglfunctions_3_3_core.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <shader.h>

#include <iostream>
#include <vector>

// graphics tools
#include "Camera.h"
#include "Light.h"
#include "Asset.h"
#include "Model.h"
#include "Mesh.h"

class OpenGLWidget : public QOpenGLWidget {
public:
	// tools
	// mouse state, POV for point of view camera and controls, MOUSE for normal mouse movement detection and no camera effect.
	enum MouseControlState { POV, MOUSE, CUSTOM };

	// add all models before you start making assets
	// a simple graphicsengine (uses multisampling x4)

	// start of class code
	QOpenGLFunctions_3_3_Core *f;

	// delayed callback for init after gl is initialized
	void(*afterGLInit)(OpenGLWidget*) = nullptr;

	Camera camera;

	Shader shader;
	Shader testCubeShader;

	// light
	Light light;

	// samples for multisampling
	int samples;

	// list of active models
	std::vector<Model*> models;

	// list of the physical models with all the transforms applied
	std::vector<Asset*> scene;

	// mouse modes
	MouseControlState mouseMode;

	bool clampMouse;
	bool pastClampMouse;

	// temp testing vars
	unsigned int VAO;
	unsigned int VBO;

	OpenGLWidget() : QOpenGLWidget() {
		setup();
		initFormat();
	}

	OpenGLWidget(QWidget* parent) : QOpenGLWidget(parent) {
		setup();
		initFormat();
	}

	void setup() {
		samples = 1;
	}

	void initFormat() {
		QSurfaceFormat format;
		format.setVersion(3, 3);
		format.setProfile(QSurfaceFormat::CoreProfile);
		format.setSamples(samples);

		setFormat(format);
	}

	void setAfterGLInit(void m(OpenGLWidget*)) {
		afterGLInit = m;
	}

	void initializeGL() override {
		f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

		// configure global opengl state
		f->glEnable(GL_DEPTH_TEST);

		if (samples > 1) {
			f->glEnable(GL_MULTISAMPLE);
		}

		// Camera
		camera = Camera(width(), height(), glm::vec3(0), true);

		// for textures
		// stbi_set_flip_vertically_on_load(true);

		// mouse control State default
		setMouseMode(MouseControlState::POV);

		// light setup
		light = Light(f, "resources/shaders/light.vs", "resources/shaders/light.fs");

		// text cube setup
		testCubeShader = Shader(f, "resources/shaders/cube.vs", "resources/shaders/cube.fs");
		//generateTestCube();

		// local shader
		// shader = Shader("resources/shaders/basic_model.vs", "resources/shaders/basic_model.fs");
		shader = Shader(f, "resources/shaders/lighted_model.vs", "resources/shaders/lighted_model.fs");
		
		f->glClearColor(0.1f, 0.1f, 0.1f, 0.1f);


		// after init
		afterGLInit(this);
	}

	void paintGL() override {
		f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

		// process input
		// optional camera input
		if (mouseMode != MouseControlState::CUSTOM) {
			//camera.processInput(window);
		}

		// prep for render
		f->glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//generateTestCube();
		//drawTestCube(glm::vec3(0,0,4));
		
		// model rendering
		shader.use();

		// if the light is valid then enter lighting mode for shader
		if (light.enabled) {
			shader.setBool("enableLighting", true);

			shader.setVec3("lightPos", light.pos);
			shader.setVec3("lightColor", light.color);
			shader.setFloat("lightBrightness", light.brightness);
			shader.setFloat("lightDistance", light.distance);
		}

		// draw assets with the corresponding model
		// draw backwards since the board is transparent and the balls and other objects need to be drawn first
		for (int i = scene.size() - 1; i >= 0; i--) {
			if (scene[i]->visible) {
				// camera stuff

				glm::mat4 projection = camera.projection;
				glm::mat4 view = camera.update();
				shader.setMat4("projection", projection);
				shader.setMat4("view", view);
				shader.setVec3("viewPos", camera.pos);

				// translate model
				glm::mat4 model = glm::mat4(1.0f);
				model = glm::translate(model, scene[i]->position);
				model = glm::rotate(model, glm::radians(scene[i]->rotation.x), glm::vec3(1.0, 0.0, 0.0));
				model = glm::rotate(model, glm::radians(scene[i]->rotation.y), glm::vec3(0.0, 1.0, 0.0));
				model = glm::rotate(model, glm::radians(scene[i]->rotation.z), glm::vec3(0.0, 0.0, 1.0));
				model = glm::scale(model, scene[i]->scale);	// it's a bit too big for our scene, so scale it down
				shader.setMat4("model", model);

				if (scene[i]->model != nullptr) {
					scene[i]->model->Draw(shader, camera);
				}
			}
		}
	}

	// set mouse event handling to update mouse struct
	void mousePressEvent(QMouseEvent* event) {
		/*
		glm::vec2 pos = glm::vec2(event->localPos().x, event->localPos().y);
		if (mouseInsideWindow(pos)) {
			mouse.pos = pos;
		}
		// let the original class use the event if it wants
		else {
			QOpenGLWidget::mousePressEvent(event);
		}
		*/

		// callback and then original class
		QOpenGLWidget::mousePressEvent(event);
	}

	// legacy mouse handling
	// switch Mouse Modes
	void setMouseMode(MouseControlState state) {
		if (state == MouseControlState::POV) {
			mouseMode = state;
			clampMouse = true;
		}
		else if (state == MouseControlState::MOUSE) {
			clampMouse = false;
			mouseMode = state;
		}
		else {
			mouseMode = state;
		}
	}

	// color is 0-1 so white is (1,1,1)
	void setLight(glm::vec3 pos, glm::vec3 color) {
		light.pos = pos;
		light.color = color;
		light.enabled = true;
	}

	Light* getLight() {
		return &light;
	}

	// asset stuff
	// add refrences to assets
	void addAsset(Asset *asset) {
		scene.push_back(asset);
	}

	// remove assets
	void removeAsset(Asset *asset) {
		for (int i = 0; i < scene.size(); i++) {
			if (scene[i] == asset) {
				scene.erase(scene.begin() + i);
				break;
			}
		}
	}

	void generateTestCube() {
		float cube[] = {
			// back
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			// front
			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			// left
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			// right
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,

			 // bottom
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,

			// top
			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
		};

		// init shaders
		// shader = Shader(f, "resources/shaders/cube.vs", "resources/shaders/cube.fs");

		// clear data to preserve memory
		f->glDeleteVertexArrays(1, &VAO);
		f->glDeleteBuffers(1, &VBO);

		// load vbo and make vao
		f->glGenVertexArrays(1, &VAO);
		f->glGenBuffers(1, &VBO);
		f->glBindVertexArray(VAO);

		f->glBindBuffer(GL_ARRAY_BUFFER, VBO);
		f->glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

		f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		f->glEnableVertexAttribArray(0);
	}

	void drawTestCube(glm::vec3 pos = glm::vec3(0)) {
		// draw it
		// use shader and set view and projection panes
		
		testCubeShader.use();
		testCubeShader.setMat4("projection", camera.projection);
		testCubeShader.setMat4("view", camera.update());

		// set coords and scale
		glm::mat4 cubeModel(1);
		cubeModel = glm::translate(cubeModel, pos);
		cubeModel = glm::scale(cubeModel, glm::vec3(1));
		testCubeShader.setMat4("model", cubeModel);
		

		// bind, draw, then reset bind to vao
		f->glBindVertexArray(VAO);
		f->glDrawArrays(GL_TRIANGLES, 0, 36);
		f->glBindVertexArray(0);
	}
};

#endif