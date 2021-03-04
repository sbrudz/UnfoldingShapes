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
		UnfoldSolution* solution;

		// milliseconds for animation to complete
		float time;

		Animation(Shape* shape, UnfoldSolution* solution, float time) {
			this->shape = shape;
			this->solution = solution;
			this->time = time;
		}
	};

	vector<Animation> animations;

	Animator() {
		animations = vector<Animation>();
	}

	// main update function for all animations
	void update() {
		for (int i = 0; i < animations.size(); i++) {
			recursiveBreadth(animations[i].solution->rootNode);

			// rebuild the mesh for each shape
			animations[i].shape->model->rebuildMeshes();
		}
	}

	void addAnimation(Shape* shape, UnfoldSolution* solution, float time) {
		animations.push_back(Animation(shape, solution, time));
	}

private:
	void recursiveUpdate(UnfoldSolution::Node* root) {
		for (int i = 0; i < root->children.size(); i++) {
			root->children[i]->data
		}
	}
};

#endif