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
	static void basicRecusivePopulation(Graph<Face>::Node* mapNode, UnfoldSolution* solution, UnfoldSolution::Node* parent) {
		for (int i = 0; i < mapNode->connections.size(); i++) {
			if (solution->findNode(solution->rootNode, mapNode->connections[i]->data) == nullptr) {
				// set new parent
				parent = solution->addNode(mapNode->connections[i]->data, parent);
				
				// recursive
				basicRecusivePopulation(mapNode->connections[i], solution, parent);
			}
		}
	}

public:
	static UnfoldSolution basic(Shape* shape) {
		// init solution with the base 
		UnfoldSolution solution = UnfoldSolution(shape->faceMap.rootNode->data);

		basicRecusivePopulation(shape->faceMap.rootNode, &solution, solution.rootNode);

		return solution;
	}
};

#endif