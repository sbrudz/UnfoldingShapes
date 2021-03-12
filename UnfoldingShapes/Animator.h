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
			progress = 0;
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
				breadthFirstUpdate(animations[i].solution->rootNode, &animations[i]);
				//recursiveUpdate(animations[i].solution->rootNode, &animations[i]);

				animations[i].progress += 1.0f / (100 * animations[i].time);
			}

			// rebuild the mesh for each shape
			animations[i].shape->model->rebuildMeshes();
		}
	}

	void addAnimation(Shape* shape, Graph<Face>* solution, float time) {
		animations.push_back(Animation(shape, solution, time));
	}

private:
	void rotateFaceAboutAxis(Face* face, Face::Axis* axis, float deltaTheta) {
		for (int i = 0; i < face->mesh->vertices.size(); i++) {
			// rotate vertices and axis
			face->mesh->vertices[i].Position = axis->rotateAbout(face->mesh->vertices[i].Position, deltaTheta);
		}

		for (int j = 0; j < face->axis.size(); j++) {
			face->axis[j]->rotateAxisAbout(axis, deltaTheta);
		}
	}

	void recursiveChildRotation(Graph<Face>::Node* root, Face::Axis* axis, float deltaTheta) {
		// rotate this node and then rotate all of the children
		rotateFaceAboutAxis(root->data, axis, deltaTheta);

		for (int i = 0; i < root->connections.size(); i++) {
			recursiveChildRotation(root->connections[i], axis, deltaTheta);
		}
	}

	// Broken (makes a cool effect though) ITS DEPTH FIRST
	void recursiveUpdate(Graph<Face>::Node* root, Animation* animation) {
		for (int i = 0; i < root->connections.size(); i++) {
			// apply rotations (find the right axis)
			Face::Axis* axis = nullptr;

			for (int x = 0; x < root->data->axis.size(); x++) {\
				// if the axis is linked to the neighbor
				if (root->data->axis[x]->neighborFace == root->connections[i]->data) {
					axis = root->data->axis[x];
				}
			}

			// proccess children
			recursiveUpdate(root->connections[i], animation);

			if (axis != nullptr) {
				recursiveChildRotation(root->connections[i], axis, 1 * axis->originalAngle / (animation->time * 100));
				//std::cout << "main: " << glm::to_string(axis->line) << " " << glm::to_string(axis->point) << std::endl;
				//std::cout << "shared: " << glm::to_string(axis->sharedAxis->line) << " " << glm::to_string(axis->sharedAxis->point) << std::endl;
			}
			else {
				/*
				for (int y = 0; y < root->connections[i]->data->axis.size(); y++) {
					std::cout << glm::to_string(root->connections[i]->data->axis[y]->line) << " " << glm::to_string(root->connections[i]->data->axis[y]->point) << std::endl;
				}
				for (int x = 0; x < root->data->axis.size(); x++) {
					std::cout << glm::to_string(root->data->axis[x]->line) << " " << glm::to_string(root->data->axis[x]->point) << std::endl;
				}
				std::cout << axis << std::endl;
				*/
			}
		}
	}

	// Current working solution
	void breadthFirstUpdate(Graph<Face>::Node* root, Animation* animation) {
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
					}
				}

				// apply to children
				if (axis != nullptr) {
					recursiveChildRotation(current->connections[i], axis, 1 * axis->originalAngle / (animation->time * 100));
				}

				queue.push_back(current->connections[i]);
			}
		}
	}
};

#endif