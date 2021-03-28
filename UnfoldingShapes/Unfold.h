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

	static void breadthPopulation(Graph<Face>::Node* root, Graph<Face>* solution) {
		vector<Graph<Face>::Node*> queue;
		vector<Graph<Face>::Node*> visited;

		queue.push_back(root);
		visited.push_back(root);

		Graph<Face>::Node* current;

		while (!queue.empty()) {
			current = queue[0];
			queue.erase(queue.begin());

			//std::cout << "New Animation Frame:" << std::endl;

			Graph<Face>::Node* currentSolutionNode = solution->findNode(root, current->data);
			for (int i = 0; i < current->connections.size(); i++) {
				bool inList = false;

				for (int x = 0; x < visited.size(); x++) {
					if (visited[x] == current->connections[i]) {
						inList = true;
						break;
					}
				}

				if (!inList) {
					solution->newNode(currentSolutionNode, current->connections[i]->data);

					visited.push_back(current->connections[i]);
					queue.push_back(current->connections[i]);
				}
			}
		}
	}

public:
	static Graph<Face>* basic(Shape* shape) {
		// init solution with the base 
		// std::cout << shape->faceMap.rootNode << std::endl;
		Graph<Face>* solution = new Graph<Face>(shape->faceMap.rootNode->data);

		basicRecusivePopulation(shape->faceMap.rootNode, solution, solution->rootNode);

		// quick fix (remove the last element of the graph)

		return solution;
	}

	static Graph<Face>* breadthUnfold(Shape* shape) {
		// init solution with the base 
		// std::cout << shape->faceMap.rootNode << std::endl;
		Graph<Face>* solution = new Graph<Face>(shape->faceMap.rootNode->data);

		breadthPopulation(shape->faceMap.rootNode, solution);

		std::cout << solution->size << std::endl;

		return solution;
	}
};

#endif