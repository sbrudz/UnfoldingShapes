#include "UnfoldingShapes.h"
#include "OpenGLWidget.h"
#include <QtWidgets/QApplication>
#include <QtWidgets/qopenglwidget.h>
#include "Runner.h"

// we have to delay the runner setup because opengl must be initialized first
Runner *runner;
UnfoldingShapes* wPointer;

void createRunner(OpenGLWidget *w);

int main(int argc, char *argv[])
{
	std::cout << "finished compilation" << std::endl;
    QApplication a(argc, argv);
    UnfoldingShapes w;
	wPointer = &w;

	w.setDelayedSetup(&createRunner);

	w.show();

	std::cout << "finished init" << std::endl;

    return a.exec();
}

void createRunner(OpenGLWidget *w) {
	runner = new Runner(w, wPointer);
}
