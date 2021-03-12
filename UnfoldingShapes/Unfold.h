#ifndef UNFOLD_H
#define UNFOLD_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <vector>

#include "Model.h"
#include "Mesh.h"

#include "Face.h"
#include "Graph.h"
#include "Shape.h"

#include "UnfoldSolution.h"

// computes unfold solutions
static class Unfold {
private:
	static void basicRecusivePopulation(Graph<Face>::Node* mapNode, Graph<Face>* solution, Graph<Face>::Node* parent) {
		for (int i = 0; i < mapNode->connections.size(); i++) {
			if (solution->findNode(solution->rootNode, mapNode->connections[i]->data) == nullptr) {
				// set new parent
				parent = solution->newNode(parent, mapNode->connections[i]->data);
				
				// recursive
				basicRecusivePopulation(mapNode->connections[i], solution, parent);
			}
		}
	}

	static void breadthRecusivePopulation(Graph<Face>::Node* mapNode, Graph<Face>* solution, Graph<Face>::Node* parent) {
		bool flag = false;

		for (int i = 0; i < mapNode->connections.size(); i++) {
			if (solution->findNode(solution->rootNode, mapNode->connections[i]->data) == nullptr) {
				// set new parent
				parent = solution->newNode(parent, mapNode->connections[i]->data);

				flag = true;
			}
		}

		if (flag) {
			return;
		}

		for (int i = 0; i < mapNode->connections.size(); i++) {
			Graph<Face>::Node* node = solution->findNode(solution->rootNode, mapNode->connections[i]->data);

			//if (solution->findNode(solution->rootNode, mapNode->connections[i]->data) == nullptr) {
				breadthRecusivePopulation(mapNode->connections[i], solution, node);
			//}
		}
	}

public:
	static Graph<Face> basic(Shape* shape) {
		// init solution with the base 
		// std::cout << shape->faceMap.rootNode << std::endl;
		Graph<Face> solution = Graph<Face>(shape->faceMap.rootNode->data);

		basicRecusivePopulation(shape->faceMap.rootNode, &solution, solution.rootNode);

		return solution;
	}

	static Graph<Face> breadthUnfold(Shape* shape) {
		// init solution with the base 
		// std::cout << shape->faceMap.rootNode << std::endl;
		Graph<Face> solution = Graph<Face>(shape->faceMap.rootNode->data);

		while (solution.size < shape->faceMap.size) {
			std::cout << solution.size << " " << shape->faceMap.size << std::endl;
			breadthRecusivePopulation(shape->faceMap.rootNode, &solution, solution.rootNode);
		}

		return solution;
	}
};

#endif