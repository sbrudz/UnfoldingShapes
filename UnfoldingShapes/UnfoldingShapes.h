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

		// enable controls
		ui.openGLWidget->installEventFilter(this);
	}

	OpenGLWidget* getGraphics() {
		return ui.openGLWidget;
	}

	// setup
	// set the callback to be called after gl has initialized within the widget
	void setDelayedSetup(void m(OpenGLWidget*)) {
		ui.openGLWidget->setAfterGLInit(m);
	}

	// the function that should be called once the runner and gl have been initialized
	void delayedSetup() {
		mouse = new Mouse();

		cameraSetup();
	}

	void linkShapes(vector<Shape*>* shapes) {
		this->shapes = shapes;

		setupBackBoard();
	}

	void linkAnimator(Animator* animator) {
		this->animator = animator;
	}

	void cameraSetup() {
		origin = glm::vec3(0);
		zoom = 12.0f;

		currentMousePos = glm::vec2(0);

		// set camera starting pos
		//ui.openGLWidget->camera.setPos(origin + glm::normalize(glm::vec3(0.0f, 0.5f, -1.0f)) * zoom);
		ui.openGLWidget->camera.setPos(origin + glm::normalize(glm::vec3(0.0f, 0.0f, -1.0f)) * zoom);

		ui.openGLWidget->camera.lookAtTarget(origin);
	}

	bool eventFilter(QObject *obj, QEvent *event) override {
		if (obj == ui.openGLWidget && (event->type() == QEvent::MouseMove 
			|| event->type() == QEvent::MouseButtonPress 
			|| event->type() == QEvent::MouseButtonRelease)) {
			updateMouse(event);
		}

		return false;
	}

	void updateMouse(QEvent* event) {
		if (mouse == nullptr) {
			return;
		}

		// Make the mouse input addative (so the camera stays in the same position when the mouse releases and then presses again)
		// this way when the button is first placed, the large jump is ignored since mouse resets the positon and pos - mouse->pos is zero again
		if (event->type() == QEvent::MouseMove) {
			QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
			glm::vec2 pos = glm::vec2(mouseEvent->localPos().x(), mouseEvent->localPos().y());
			currentMousePos = currentMousePos + pos - mouse->pos;
		}

		mouse->update(event);

		// apply camera rotation for the viewer and transformation
		ui.openGLWidget->camera.mouseInputPOV(currentMousePos.x, currentMousePos.y);

		ui.openGLWidget->camera.setPos(origin + ui.openGLWidget->camera.Front * -1.0f * zoom);
	}

	bool mouseInsideWindow(glm::vec2 pos) {
		if (pos.x > 0 && pos.y > 0 && pos.x < width() && pos.y < height()) {
			return true;
		}

		return false;
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
		shape->asset->position = origin;

		// stop current animation of the focused shape
		if (focusedShape != nullptr) {
			animator->getAnimation(focusedShape)->stop();
		}

		focusedShape = shape;
		focusedShape->asset->setRotation(glm::vec3(0));

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
		backboard = new Backboard(this, shapes, glm::vec3(0, 10, 50));
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

	struct Backboard : QObject {
		UnfoldingShapes* parent;

		vector<Shape*>* shapes;

		// update timer for backboard animation
		QTimer* timer;
		int updateLoopHZ = 30;
		float rotationSpeed = 2;

		// top left
		glm::vec3 position;

		// width of board before wrapping to the bottom
		int width;

		//seperation between the shapes
		glm::vec2 seperation;

		// position is the top center of the backboard
		Backboard(UnfoldingShapes *parent, vector<Shape*>* shapes, glm::vec3 position, int width = 5, glm::vec2 seperation = glm::vec2(5,5)) : QObject(nullptr) {
			this->parent = parent;
			this->shapes = shapes;
			this->width = width;
			this->seperation = seperation;
			this->position = position;

			applyTransform();

			// setup update loop
			timer = new QTimer(this);
			QObject::connect(timer, &QTimer::timeout, this, &Backboard::updateLoop);
			timer->start(updateLoopHZ);
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

		void updateLoop() {
			// rotate the shapes on the backboard if it is not focused
			for (int i = 0; i < shapes->size(); i++) {
				if ((*shapes)[i] != parent->focusedShape) {
					(*shapes)[i]->asset->setRotation((*shapes)[i]->asset->rotation + glm::normalize(glm::vec3(1, 2, 3)) * rotationSpeed);
				}
				else {
					// do nothing
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

	// mouse stuff

	struct Mouse {
		UnfoldingShapes* parent = nullptr;

		glm::vec2 pos;

		bool left;
		bool right;

		Mouse() {
			this->parent = parent;

			left = false;
			right = true;

			pos = glm::vec2(0);
		}

		void update(QEvent* event) {
			QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);

			if (event->type() == QEvent::MouseButtonPress) {
				left = true;
			}

			if (event->type() == QEvent::MouseButtonRelease) {
				left = false;
			}

			pos = glm::vec2(mouseEvent->localPos().x(), mouseEvent->localPos().y());
		}
	};

	Mouse* mouse = nullptr;

	glm::vec2 currentMousePos;
};

#endif
