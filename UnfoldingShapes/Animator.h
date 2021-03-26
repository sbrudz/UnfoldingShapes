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
#include "Skybox.h"
#include "TextManager.h"
#include "GraphicsEngine.h"

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

	vector<Animation> animations;

	Animator() {
		animations = vector<Animation>();
	}

	// main update function for all animations
	void update() {
		for (int i = 0; i < animations.size(); i++) {
			if (animations[i].progress < 1.0f) {
				// first revert
				animations[i].shape->revert();

				breadthFirstUpdate(animations[i].shape, animations[i].solution->rootNode, animations[i].progress);
				//recursiveUpdate(animations[i].solution->rootNode, &animations[i]);

				//std::cout << animations[i].progress << std::endl;
				animations[i].progress += 2.0f * 2.0f / (100 * animations[i].time);
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

private:
	void recursiveChildCompilation(vector<Face*>* list, Graph<Face>::Node* root) {
		// add face of node
		list->push_back(root->data);

		// process children of node
		for (int i = 0; i < root->connections.size(); i++) {
			recursiveChildCompilation(list, root->connections[i]);
		}
	}

	// Current working solution
	void breadthFirstUpdate(Shape* shape, Graph<Face>::Node* root, float progress) {
		vector<Graph<Face>::Node*> queue;

		queue.push_back(root);

		Graph<Face>::Node* current;

		while (!queue.empty()) {
			current = queue[0];
			queue.erase(queue.begin());

			for (int i = 0; i < current->connections.size(); i++) {
				// find axis
				Face::Axis* axis = nullptr;

				for (int x = 0; x < current->data->axis.size(); x++) {
					if (current->data->axis[x]->neighborFace == current->connections[i]->data) {
						axis = current->data->axis[x];

						break;
					}
				}

				// apply to children
				if (axis != nullptr) {
					// use the method to add all child faces attatched to the current face for the transformation of the shape.
					vector<Face*> appliedFaces;
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