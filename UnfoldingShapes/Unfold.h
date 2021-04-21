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

//prototypes
template<class RandomIt>
void random_shuffle(RandomIt first, RandomIt last);

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

	static void randomBreadthPopulation(Graph<Face>::Node* root, Graph<Face>* solution) {
		vector<Graph<Face>::Node*> queue;
		vector<Graph<Face>::Node*> visited;

		queue.push_back(root);
		visited.push_back(root);

		Graph<Face>::Node* current;

		// random seed
		srand(time(NULL));

		while (!queue.empty()) {
			current = queue[0];
			queue.erase(queue.begin());

			Graph<Face>::Node* currentSolutionNode = solution->findNode(root, current->data);

			// randomly shuffle the connections order
			vector<Graph<Face>::Node*> randConnections = current->connections;
			random_shuffle(randConnections.begin(), randConnections.end());

			for (int i = 0; i < randConnections.size(); i++) {
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

	static Graph<Face>* randomBreadthUnfold(Shape* shape) {
		// init solution with the base 
		// std::cout << shape->faceMap.rootNode << std::endl;
		srand(time(NULL));

		Graph<Face>* solution = new Graph<Face>(shape->faceMap.rootNode->data);

		randomBreadthPopulation(shape->faceMap.rootNode, solution);

		std::cout << solution->size << std::endl;

		return solution;
	}
};


template<class RandomIt>
void random_shuffle(RandomIt first, RandomIt last)
{
	typename std::iterator_traits<RandomIt>::difference_type i, n;
	n = last - first;
	for (i = n - 1; i > 0; --i) {
		using std::swap;
		swap(first[i], first[std::rand() % (i + 1)]);
		// rand() % (i+1) isn't actually correct, because the generated number
		// is not uniformly distributed for most values of i. A correct implementation
		// will need to essentially reimplement C++11 std::uniform_int_distribution,
		// which is beyond the scope of this example.
	}
}

#endif