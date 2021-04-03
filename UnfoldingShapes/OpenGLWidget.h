#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include "Runner.h"
#include <qopenglwidget.h>

class OpenGLWidget : public QOpenGLWidget {
public:
	Runner* program;

	OpenGLWidget() {
		
	}

	void initializeGL() override {
		// start program
		program = new Runner();
	}

	void paintGL() override {
		program->frame();
	}
};

#endif