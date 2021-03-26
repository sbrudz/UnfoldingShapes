#ifndef GRAPH_H
#define GRAPH_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <vector>
#include "Face.h"

using namespace std;

template<class T>
class Graph {
public:
	// prototype
	struct Node;

	// vars
	int size;

	Node* rootNode = nullptr;

	struct Node {
		int id;
		T* data;

		Graph<T>* graph;

		std::vector<Node*> connections;
	};

	// depth first search for node with data (recursive)
	// enable searching means that there will be no overlap of the indexed nodes.
	struct Node* findNode(Node* root, T* data, bool searching = false) {
		static vector<Node*> searchedNodes;

		// empty if starting a new search
		if (searching == false) {
			searchedNodes = vector<Node*>();
		}

		if (root != nullptr) {
			// first compare root and then children
			if (root->data == data) {
				return root;
			}

			for (int i = 0; i < root->connections.size(); i++) {
				if (root->connections[i]->data == data) {
					return root->connections[i];
				}

				bool contains = false;
				for (int j = 0; j < searchedNodes.size(); j++) {
					if (searchedNodes[j] == root->connections[i]) {
						contains = true;
						break;
					}
				}

				if (contains == false) {
					searchedNodes.push_back(root->connections[i]);

					Node* attempt = findNode(root->connections[i], data, true);
					if (attempt != nullptr) {
						return attempt;
					}
				}
			}
		}

		return nullptr;
	}

	struct Node* newRootNode(T* data) {
		Node* node = new Node();

		node->id = size++;
		node->data = data;

		node->graph = this;

		rootNode = node;

		return node;
	}

	// makes a new node (specify the parent and then the data) (automatically adds connection to root as parent)
	struct Node* newNode(Node* root, T* data, bool twoWayConnections = false) {
		// create node if root is valid
		Node* node = findNode(rootNode, data);

		if (node == nullptr) {
			node = new Node();

			node->id = size++;
			node->data = data;

			node->graph = this;

			// connect parent and child
			root->connections.push_back(node);

			if (twoWayConnections) {
				node->connections.push_back(root);
			}
		}

		// if already exists then just connect the two (double check they are not connected already)
		else {
			// check this node
			bool alreadyExists = false;
			for (int i = 0; i < root->connections.size(); i++) {
				if (node == root->connections[i]) {
					alreadyExists = true;
					break;
				}
			}

			if (!alreadyExists) {
				root->connections.push_back(node);
			}

			// check child
			// cancel if not a two way connection setup
			if (twoWayConnections) {
				alreadyExists = false;
				for (int i = 0; i < node->connections.size(); i++) {
					if (root == node->connections[i]) {
						alreadyExists = true;
						break;
					}
				}

				if (!alreadyExists) {
					node->connections.push_back(root);
				}
			}
		}

		return node;
	}

	// You must initialize with the first Node data
	Graph() {
		size = 0;
		rootNode = nullptr;
	}

	Graph(T* data) {
		size = 0;
		rootNode = newRootNode(data);
	}
};

#endif