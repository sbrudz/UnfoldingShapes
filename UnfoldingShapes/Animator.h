#ifndef ANIMATOR_H
#define ANIMATOR_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <vector>

// graphics tools
#include "Camera.h"
#include "Light.h"
#include "Asset.h"
#include "Model.h"
#include "Mesh.h"

#include "Face.h"
#include "Graph.h"
#include "UnfoldSolution.h"
#include "Unfold.h"

class Animator {
public:
	struct Animation {
		Shape* shape;
		Graph<Face>* solution;

		// milliseconds for animation to complete
		float time;

		float progress;

		Animation(Shape* shape, Graph<Face>* solution, float time) {
			this->shape = shape;
			this->solution = solution;
			this->time = time;
			progress = 0.0f;
		}
	};

	Animator() {
		activeAlgorithm = 1;
		paused = false;
		speed = 2.0f;

		animations = vector<Animation>();
	}

	// main update function for all animations
	void update(GraphicsEngine* graphics) {
		this->graphics = graphics;

		for (int i = 0; i < animations.size(); i++) {
			if (animations[i].progress < 0.0f) {
				// first revert
				animations[i].shape->revert();

				animations[i].progress = 0.0f;
			}
			else if (animations[i].progress < 1.0f) {
				// first revert
				animations[i].shape->revert();

				// identify which algorithm to use
				switch (activeAlgorithm) {
				case 0: {
					stepBasedUpdate(animations[i].shape, animations[i].solution->rootNode, animations[i].progress);
					break;
				}
				case 1: {
					breadthFirstUpdate(animations[i].shape, animations[i].solution->rootNode, animations[i].progress);
					break;
				}
				}

				if (!paused) {
					//std::cout << animations[i].progress << std::endl;
					animations[i].progress += speed / (100 * animations[i].time);
				}
			}
			else if (animations[i].progress > 1.0f) {
				animations[i].progress = 1.0f;

				// first revert
				animations[i].shape->revert();

				breadthFirstUpdate(animations[i].shape, animations[i].solution->rootNode, animations[i].progress);
			}

			// rebuild the mesh for each shape
			animations[i].shape->model->rebuildMeshes();
		}
	}

	void addAnimation(Shape* shape, Graph<Face>* solution, float time) {
		animations.push_back(Animation(shape, solution, time));
	}

	bool isPaused() {
		return paused;
	}

	void pause() {
		paused = true;
	}

	void play() {
		paused = false;
	}

	// input a decimal 0.0-1.0 to represet percentage
	void scrub(float delta) {
		for (int i = 0; i < animations.size(); i++) {
			animations[i].progress += delta;
		}
	}

	void incrementSpeed(float delta) {
		speed += delta;
	}

	void shuffleAlgorithm() {
		int algorithmCount = 2;

		activeAlgorithm = (activeAlgorithm + 1) % algorithmCount;
	}

private:
	vector<Animation> animations;

	// controls related stuff
	bool paused;
	float speed;

	GraphicsEngine* graphics;

	// which of the available algortihms is being used
	int activeAlgorithm;

	void recursiveChildCompilation(vector<Face*>* list, Graph<Face>::Node* root) {
		// add face of node
		list->push_back(root->data);

		// process children of node
		for (int i = 0; i < root->connections.size(); i++) {
			recursiveChildCompilation(list, root->connections[i]);
		}
	}

	void stepBasedUpdate(Shape* shape, Graph<Face>::Node* root, float progress) {
		float miniProgress = 1.0f / root->graph->size;

		vector<Graph<Face>::Node*> queue;

		queue.push_back(root);

		Graph<Face>::Node* current;

		int index = 0;

		while (queue.size() <= floor(progress / miniProgress)) {
			current = queue[index];

			//std::cout << "New Animation Frame:" << std::endl;

			for (int i = 0; i < current->connections.size(); i++) {
				queue.push_back(current->connections[i]);
			}

			index += 1;
		}

		// catchup all the queue indicies less than current
		for (int z = 0; z < queue.size() && z != floor(progress / miniProgress); z++) {
			current = queue[z];

			// find axis
			Face::Axis* axis = nullptr;

			for (int i = 0; i < current->connections.size(); i++) {
				for (int x = 0; x < current->data->axis.size(); x++) {
					if (current->data->axis[x]->neighborFace == current->connections[i]->data) {
						axis = current->data->axis[x];

						//axis->print();

						break;
					}
				}

				// apply to children
				if (axis != nullptr) {
					// use the method to add all child faces attatched to the current face for the transformation of the shape.
					vector<Face*> appliedFaces = vector<Face*>();
					recursiveChildCompilation(&appliedFaces, current->connections[i]);

					shape->transform(1 * (axis->originalAngle), axis, appliedFaces);
				}
			}
		}

		// handle latest update
		current = queue[floor(progress / miniProgress)];

		// find axis
		Face::Axis* axis = nullptr;

		for (int i = 0; i < current->connections.size(); i++) {
			for (int x = 0; x < current->data->axis.size(); x++) {
				if (current->data->axis[x]->neighborFace == current->connections[i]->data) {
					axis = current->data->axis[x];

					//axis->print();

					break;
				}
			}

			// apply to children
			if (axis != nullptr) {
				// use the method to add all child faces attatched to the current face for the transformation of the shape.
				vector<Face*> appliedFaces = vector<Face*>();
				recursiveChildCompilation(&appliedFaces, current->connections[i]);

				shape->transform(1 * (axis->originalAngle) * (fmod(progress, miniProgress) / miniProgress), axis, appliedFaces);
			}
		}
	}

	// Current working solution
	void breadthFirstUpdate(Shape* shape, Graph<Face>::Node* root, float progress) {
		vector<Graph<Face>::Node*> queue;

		queue.push_back(root);

		Graph<Face>::Node* current;

		// debugging tool to identify axis
		int facesVisited = 0;

		while (!queue.empty()) {
			current = queue[0];
			queue.erase(queue.begin());

			//std::cout << "New Animation Frame:" << std::endl;

			for (int i = 0; i < current->connections.size(); i++) {
				facesVisited += 1;

				// find axis
				Face::Axis* axis = nullptr;

				for (int x = 0; x < current->data->axis.size(); x++) {
					if (current->data->axis[x]->neighborFace == current->connections[i]->data) {
						axis = current->data->axis[x];

						//axis->print();

						break;
					}
				}

				// apply to children
				if (axis != nullptr) {
					// use the method to add all child faces attatched to the current face for the transformation of the shape.
					vector<Face*> appliedFaces = vector<Face*>();
					recursiveChildCompilation(&appliedFaces, current->connections[i]);

					// apply to the shape
					shape->transform(1 * (axis->originalAngle) * progress, axis, appliedFaces);

					queue.push_back(current->connections[i]);
				}
			}
		}
	}
};

#endif