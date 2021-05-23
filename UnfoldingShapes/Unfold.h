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
void recursiveChildCompilation(vector<Face*>* list, Graph<Face>::Node* root);
void breadthFirstUpdate(Shape* shape, Graph<Face>::Node* root, float progress);

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

	static void randomBasicRecusivePopulation(Graph<Face>::Node* mapNode, Graph<Face>* solution, Graph<Face>::Node* parent) {
		// randomly shuffle the connections order
		vector<Graph<Face>::Node*> randConnections = mapNode->connections;
		random_shuffle(randConnections.begin(), randConnections.end());

		for (int i = 0; i < randConnections.size(); i++) {
			if (solution->findNode(solution->rootNode, randConnections[i]->data) == nullptr) {
				// set new parent
				parent = solution->newNode(parent, randConnections[i]->data);

				// recursive
				basicRecusivePopulation(randConnections[i], solution, parent);
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

			Graph<Face>::Node* currentSolutionNode = solution->findNode(solution->rootNode, current->data);
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

		while (!queue.empty()) {
			current = queue[0];
			queue.erase(queue.begin());

			Graph<Face>::Node* currentSolutionNode = solution->findNode(solution->rootNode, current->data);

			// randomly shuffle the connections order
			vector<Graph<Face>::Node*> randConnections = current->connections;
			random_shuffle(randConnections.begin(), randConnections.end());

			for (int i = 0; i < randConnections.size(); i++) {
				bool inList = false;

				for (int x = 0; x < visited.size(); x++) {
					if (visited[x] == randConnections[i]) {
						inList = true;
						break;
					}
				}

				if (!inList) {
					solution->newNode(currentSolutionNode, randConnections[i]->data);

					visited.push_back(randConnections[i]);
					queue.push_back(randConnections[i]);
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

	static Graph<Face>* randomBasic(Shape* shape) {
		// init solution with the base 
		// std::cout << shape->faceMap.rootNode << std::endl;

		Graph<Face>* solution = new Graph<Face>(shape->faceMap.rootNode->data);

		randomBasicRecusivePopulation(shape->faceMap.rootNode, solution, solution->rootNode);

		return solution;
	}

	static Graph<Face>* breadthUnfold(Shape* shape) {
		// init solution with the base 
		// std::cout << shape->faceMap.rootNode << std::endl;

		Graph<Face>* solution = new Graph<Face>(shape->faceMap.rootNode->data);

		breadthPopulation(shape->faceMap.rootNode, solution);

		// std::cout << solution->size << std::endl;

		return solution;
	}

	static Graph<Face>* randomBreadthUnfold(Shape* shape) {
		// init solution with the base 
		// std::cout << shape->faceMap.rootNode << std::endl;

		Graph<Face>* solution = new Graph<Face>(shape->faceMap.rootNode->data);

		randomBreadthPopulation(shape->faceMap.rootNode, solution);

		// std::cout << solution->size << std::endl;

		return solution;
	}

	// returns the xy size of an unfold on a flat plane (returns "0,0" if there are no vertices)
	// Shape must have an unfold Assigned!
	// Assumes that the shape is rotated so the root unfold node is perfectly aligned with the xz plane
	static glm::vec2 findUnfoldSize(Shape* shape) {
		// find the position of each vertex in the unraveled 
		float minx = 0;
		float miny = 0;
		float maxx = 0;
		float maxy = 0;

		// unwrap the shape to measure
		if (shape->unfold->rootNode) {
			return glm::vec2(0);
		}

		// must ensure that the shape is normalized before calculations (reverted-see shape transformation struct)
		shape->revert();
		breadthFirstUpdate(shape, shape->unfold->rootNode, 1.0);

		for (int i = 0; i < shape->faces.size(); i++) {
			vector<Vertex>* vertices = &(shape->faces[i]->mesh->vertices);
			
			for (int j = 0; j < vertices->size(); j++) {
				glm::vec3 pos = (*vertices)[j].Position;

				if (pos.x < minx) {
					minx = pos.x;
				}
				if (pos.x > maxx) {
					maxx = pos.x;
				}
				if (pos.z < miny) {
					miny = pos.z;
				}
				if (pos.z > maxy) {
					maxy = pos.z;
				}
			}
		}

		// reset shape to defualt shape and return
		shape->revert();
		return glm::vec2(maxx-minx, maxy-miny);
	}
};

void recursiveChildCompilation(vector<Face*>* list, Graph<Face>::Node* root) {
	// add face of node
	list->push_back(root->data);

	// process children of node
	for (int i = 0; i < root->connections.size(); i++) {
		recursiveChildCompilation(list, root->connections[i]);
	}
}

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

template<class RandomIt>
void random_shuffle(RandomIt first, RandomIt last)
{
	srand(time(NULL));
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