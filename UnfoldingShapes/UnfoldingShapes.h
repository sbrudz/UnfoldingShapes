#ifndef UNFOLDINGSHAPES_H
#define UNFOLDINGSHAPES_H

#include <QtWidgets/QMainWindow>
#include "ui_UnfoldingShapes.h"
#include "OpenGLWidget.h"

class UnfoldingShapes : public QMainWindow
{
    Q_OBJECT

public:
    UnfoldingShapes(QWidget *parent = Q_NULLPTR) : QMainWindow(parent)
	{
		ui.setupUi(this);
	}

	OpenGLWidget* getGraphics() {
		return ui.openGLWidget;
	}

private:
    Ui::UnfoldingShapesClass ui;
};

#endif
