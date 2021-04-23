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

		connect(ui.applyProperties, &QPushButton::released, this, &UnfoldingShapes::applySettings);
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

	// runtime stuff
	void applySettings() {
		// retrieve info

		// make sure a shape is selected in the menu
		if (ui.listWidget->currentRow() != -1) {
			Shape* current = (*shapes)[ui.listWidget->currentRow()];

			int unfoldSetting = ui.unfoldMethodInput->currentIndex();
			int animationSetting = ui.animationMethodInput->currentIndex();
			float speed = ui.speedInput->value();
			float scale = ui.scaleInput->value();

			setUnfold(current, unfoldSetting);

			Animator::Animation* animation = animator->getAnimation(current);
			animation->setAlgorithm(animationSetting);
			animation->speed = speed;

			animation->progress = 0;
		}
	}

	// utility
	bool setUnfold(Shape* shape, int index) {
		switch (index) {
		case 0:
			shape->setUnfold(Unfold::basic(shape));
			break;
		case 1:
			shape->setUnfold(Unfold::randomBasic(shape));
			break;
		case 2:
			shape->setUnfold(Unfold::breadthUnfold(shape));
			break;
		case 3:
			shape->setUnfold(Unfold::randomBreadthUnfold(shape));
			break;
		default:
			return false;
			break;
		}

		return true;
	}

	void addShapeToList(Shape* shape) {
		ui.listWidget->addItem(shape->name.c_str());

		// apply settings for base setup
		// add the animation if the unfold generates successfully.
		if (setUnfold(shape, 2)) {
			animator->addAnimation(shape);
		}
	}

private:
    Ui::UnfoldingShapesClass ui;

	vector<Shape*>* shapes;
	Animator* animator;
};

#endif
