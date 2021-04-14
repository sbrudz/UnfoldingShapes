#include "UnfoldingShapes.h"
#include "OpenGLWidget.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/qopenglwidget.h>
#include "Runner.h"

int main(int argc, char *argv[])
{

	std::cout << "finished compilation" << std::endl;
    QApplication a(argc, argv);
    UnfoldingShapes w;

	Runner runner(w.getGraphics());

	//OpenGLWidget opengl;
	//opengl.show();
	w.show();

	std::cout << "finished init" << std::endl;

    return a.exec();
}
