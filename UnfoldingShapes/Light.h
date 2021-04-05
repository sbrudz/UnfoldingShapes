#ifndef LIGHT_H
#define LIGHT_H

#include <qopenglwidget.h>
#include <qopenglfunctions_3_3_core.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <shader.h>

#include "Camera.h"

#include <vector>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

//must have valid shader to be enabled
class Light {
public:
	QOpenGLFunctions_3_3_Core *f;

	//the difference is that enabled is only true if the light's component settings are valid which include position and that is used for calculations by other shaders
	//visible means that all mathematical components are set but you can change whether or not you want it to render
	//enabled
	bool enabled;

	//visible
	bool visible;

	glm::vec3 pos;
	glm::vec3 color;
	//1 is normal
	float brightness;
	//the effective distance of the light source
	float distance;

	unsigned int VBO;
	unsigned int VAO;

	Shader shader;

	Light() {
		enabled = false;
		visible = true;
	}

	//remember to set pos and enable the light
	Light(QOpenGLFunctions_3_3_Core *f, const char* vs, const char* fs) {
		this->f = f;

		this->pos = glm::vec3(10);
		this->color = glm::vec3(1,1,1);

		setup(vs, fs);

		brightness = 1;
		distance = 300;

		enabled = false;
		visible = true;
	}

	Light(QOpenGLFunctions_3_3_Core *f, const char* vs, const char* fs, glm::vec3 pos, glm::vec3 color) {
		this->f = f;

		this->pos = pos;
		this->color = color;

		setup(vs, fs);

		brightness = 1;
		distance = 300;

		enabled = true;
		visible = true;
	}

	//include shader files and automatically enables the light for drawing
	void setup(const char* vs, const char* fs) {

		shader = Shader(f, vs, fs);

		float cube[] = {
			//back
			-0.5f, -0.5f, -0.5f, 0.75f, 0.3333f, 0.0f,  0.0f, -1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.3333f, 0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 0.6666f, 0.0f,  0.0f, -1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 0.6666f, 0.0f,  0.0f, -1.0f,
			-0.5f,  0.5f, -0.5f, 0.75f, 0.6666f, 0.0f,  0.0f, -1.0f,
			-0.5f, -0.5f, -0.5f, 0.75f, 0.3333f, 0.0f,  0.0f, -1.0f,

			//front
			-0.5f, -0.5f,  0.5f,  0.25f, 0.3333f,  0.0f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.50f, 0.3333f,  0.0f,  0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  0.50f, 0.6666f,  0.0f,  0.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  0.50f, 0.6666f,  0.0f,  0.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.25f, 0.6666f,  0.0f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.25f, 0.3333f,  0.0f,  0.0f, 1.0f,

			//left
			-0.5f, -0.5f, -0.5f,  0.0f, 0.3333f,  -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f,  0.5f, 0.25f, 0.3333f,  -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, 0.25f, 0.6666f,  -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, 0.25f, 0.6666f,  -1.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 0.6666f,  -1.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.3333f,  -1.0f,  0.0f,  0.0f,

			//right
			 0.5f, -0.5f, -0.5f,  0.50f, 0.3333f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.75f, 0.3333f,  1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.75f, 0.6666f,  1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.75f, 0.6666f,  1.0f,  0.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.50f, 0.6666f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.50f, 0.3333f,  1.0f,  0.0f,  0.0f,

			 //bottom
			-0.5f, -0.5f, -0.5f,  0.25f, 0.0f,    0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.50f, 0.0f,    0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.50f, 0.3333f, 0.0f, -1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.50f, 0.3333f, 0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.25f, 0.3333f, 0.0f, -1.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.25f, 0.0f,    0.0f, -1.0f,  0.0f,

			//top
			-0.5f,  0.5f, -0.5f,  0.25f, 0.6666f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.50f, 0.6666f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.50f, 1.0f,     0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.50f, 1.0f,     0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.25f, 1.0f,     0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.25f, 0.6666f,  0.0f,  1.0f,  0.0f
		};

		//compile values
		//float vertices[138240];

		//make the light cube
		f->glGenVertexArrays(1, &VAO);
		f->glGenBuffers(1, &VBO);
		f->glBindVertexArray(VAO);

		f->glBindBuffer(GL_ARRAY_BUFFER, VBO);
		f->glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);

		f->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		f->glEnableVertexAttribArray(0);

		enabled = true;
	}

	void render(glm::mat4 proj, glm::mat4 view) {
		QOpenGLFunctions_3_3_Core *f = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_3_3_Core>();

		if (enabled && visible) {
			//draw light source
			shader.use();
			shader.setMat4("projection", proj);
			shader.setMat4("view", view);
			glm::mat4 lightModel(1);
			lightModel = glm::translate(lightModel, pos);
			lightModel = glm::scale(lightModel, glm::vec3(1)); //change cube size
			shader.setMat4("model", lightModel);

			f->glBindVertexArray(VAO);
			f->glDrawArrays(GL_TRIANGLES, 0, 36);
			f->glBindVertexArray(0);
		}
	}
};

#endif