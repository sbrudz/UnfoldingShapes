#ifndef UNFOLDINGSHAPES_H
#define UNFOLDINGSHAPES_H

#include <QtWidgets/QMainWindow>
#include <QMouseEvent>
#include <QtCore/qtimer.h>
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

		// local init (the rest must be delayed because gl initializes after this)
		focusedShape = nullptr;

		// apply button
		connect(ui.applyProperties, &QPushButton::released, this, &UnfoldingShapes::applySettings);

		// select shape in list
		connect(ui.listWidget, &QListWidget::itemClicked, this, &UnfoldingShapes::selectShape);
	}

	OpenGLWidget* getGraphics() {
		return ui.openGLWidget;
	}

	// setup
	// set the callback to be called after gl has initialized within the widget
	void setDelayedSetup(void m(OpenGLWidget*)) {
		ui.openGLWidget->setAfterGLInit(m);
	}

	// set the callback for when mouse events occur
	void setMouseUpdateCallback(void m(QMouseEvent*)) {
		ui.openGLWidget->setMouseUpdateCallback(m);
	}

	// the function that should be called once the runner and gl have been initialized
	void delayedSetup() {
		mouse = new Mouse();

		cameraSetup();
	}

	void cameraSetup() {
		origin = glm::vec3(0);
		zoom = 4.0f;

		// set camera starting pos
		//ui.openGLWidget->camera.setPos(origin + glm::vec3(2.0f, 1.0f, -2.0f) * zoom);
		ui.openGLWidget->camera.setPos(origin + glm::vec3(0.0f, 0.0f, -3.0f) * zoom);
		
		ui.openGLWidget->camera.lookAtTarget(origin);

		// setup controller loop
		/*
		controlHZ = 60;

		controlsTimer = new QTimer(this);
		QObject::connect(controlsTimer, &QTimer::timeout, this, &UnfoldingShapes::updateControls);
		controlsTimer->start(controlHZ);
		*/
	}

	void linkShapes(vector<Shape*>* shapes) {
		this->shapes = shapes;

		setupBackBoard();
	}

	void linkAnimator(Animator* animator) {
		this->animator = animator;
	}

	void updateMouseControls(QMouseEvent* event) {
		if (mouse == nullptr) {
			return;
		}

		// mouse.update(event);

		// apply camera rotation for the viewer
		ui.openGLWidget->camera.mouseInputPOV();

	}

	// runtime shape viewer things
	void selectShape(QListWidgetItem* item) {
		// ignore if nothing is selected
		if (ui.listWidget->currentRow() != -1) {
			// set current selection
			Shape* shape = (*shapes)[ui.listWidget->currentRow()];

			focusShape(shape);
		}
	}

	// assumes shape pointer is already added to the shapes list
	void focusShape(Shape* shape) {
		// check if the client is emptying the focus shape
		if (shape == nullptr) {
			focusedShape = nullptr;
			return;
		}

		// move the current focus back to its position
		backboard->applyTransform();

		// rotate so it is facing the correct way in the viewer
		shape->asset->setRotation(glm::vec3(330, 45, 0));
		shape->asset->position = origin;

		// stop current animation of the focused shape
		if (focusedShape != nullptr) {
			animator->getAnimation(focusedShape)->stop();
		}

		focusedShape = shape;

		// autostart animation
		applySettings();
	}

	// runtime stuff
	void applySettings() {
		// retrieve info

		// make sure a shape is selected in the menu
		if (focusedShape != nullptr) {
			Shape* current = focusedShape;

			int unfoldSetting = ui.unfoldMethodInput->currentIndex();
			int animationSetting = ui.animationMethodInput->currentIndex();
			float speed = ui.speedInput->value();
			float scale = ui.scaleInput->value();

			setUnfold(current, unfoldSetting);

			Animator::Animation* animation = animator->getAnimation(current);
			animation->setAlgorithm(animationSetting);
			animation->speed = speed;

			animation->progress = 0;

			animation->play();
		}
	}

	void setupBackBoard() {
		backboard = new Backboard(shapes, glm::vec3(0, 10, 50));
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
			animator->addAnimation(shape, true);
		}

		backboard->applyTransform();
	}

	struct Backboard {
		vector<Shape*>* shapes;

		// top left
		glm::vec3 position;

		// width of board before wrapping to the bottom
		int width;

		//seperation between the shapes
		glm::vec2 seperation;

		// position is the top center of the backboard
		Backboard(vector<Shape*>* shapes, glm::vec3 position, int width = 5, glm::vec2 seperation = glm::vec2(5,5)) {
			this->shapes = shapes;
			this->width = width;
			this->seperation = seperation;
			this->position = position;

			applyTransform();
		}

		// move all the assets of the shapes to their proper position
		void applyTransform() {
			glm::vec3 pos = getTopLeft(position);

			int col = 0;
			int row = 0;

			for (int i = 0; i < shapes->size(); i++) {
				(*shapes)[i]->asset->setPosition(pos + glm::vec3(col*seperation.x, -1 * row * seperation.y, 0));
				(*shapes)[i]->asset->setRotation(glm::vec3(0));

				col++;

				// new line
				if (col >= width) {
					col = 0;
					row++;
				}
			}
		}

		// utility
		// transform so the top left of the board is marked for transforms
		glm::vec3 getTopLeft(glm::vec3 ) {
			return position - glm::vec3((width - 1) * this->seperation.x, 0, 0);
		}
	};

private:
    Ui::UnfoldingShapesClass ui;

	vector<Shape*>* shapes;
	Animator* animator;

	// viewer pointers
	Shape* focusedShape;

	Backboard* backboard;

	// camera settings
	glm::vec3 origin;
	float zoom;

	struct Mouse {
		glm::vec2 pos;

		bool left;
		bool right;

		Mouse() {
			left = false;
			right = true;

			pos = glm::vec2(0);
		}

		void update(QMouseEvent* event) {
			if (event->button() == Qt::LeftButton && event->modifiers().) {
				left = true;
			}

			pos = glm::vec2(event->localPos().x, event->localPos().y);
		}
	};

	Mouse* mouse = nullptr;

	// control loop
	QTimer* controlsTimer;
	int controlHZ;
};

#endif
