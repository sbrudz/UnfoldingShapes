#ifndef UNFOLDINGSHAPES_H
#define UNFOLDINGSHAPES_H

#include <QtWidgets/QMainWindow>
#include "ui_UnfoldingShapes.h"
#include "OpenGLWidget.h"

#include "Shape.h"
#include "Animator.h"

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

	// setup
	void delayedSetup(void m(OpenGLWidget*)) {
		ui.openGLWidget->setAfterGLInit(m);
	}

	void linkShapes(vector<Shape*>* shapes) {
		this->shapes = shapes;
	}

	void linkAnimator(Animator* animator) {
		this->animator = animator;
	}

	void addShapeToList(Shape* shape) {
		ui.listWidget->addItem(shape->name.c_str());
	}

private:
    Ui::UnfoldingShapesClass ui;

	vector<Shape*>* shapes;
	Animator* animator;
};

#endif
