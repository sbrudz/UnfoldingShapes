#include "UnfoldingShapes.h"
#include "OpenGLWidget.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/qopenglwidget.h>

#include "Runner.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    UnfoldingShapes w;
    w.show();

	//OpenGLWidget opengl;
	//opengl.show();

    return a.exec();
}
