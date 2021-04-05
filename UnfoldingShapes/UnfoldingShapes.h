#ifndef UNFOLDINGSHAPES_H
#define UNFOLDINGSHAPES_H

#include <QtWidgets/QMainWindow>
#include "ui_UnfoldingShapes.h"

class UnfoldingShapes : public QMainWindow
{
    Q_OBJECT

public:
    UnfoldingShapes(QWidget *parent = Q_NULLPTR) : QMainWindow(parent)
	{
		ui.setupUi(this);
	}

	void setOpenGLWidget(QOpenGLWidget* w) {
		ui.openGLWidget = w;
	}

private:
    Ui::UnfoldingShapesClass ui;
};

#endif
