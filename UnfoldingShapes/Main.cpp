#include "UnfoldingShapes.h"
#include "OpenGLWidget.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/qopenglwidget.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //UnfoldingShapes w;

	OpenGLWidget opengl;
	//w.setOpenGLWidget(new OpenGLWidget());
	opengl.show();
	//w.show();

    return a.exec();
}
