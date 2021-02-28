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
	// prototype
	struct Node;

	// vars
	int size;

	struct Node {
		int id;
		T* data;

		std::vector<Node*> connections;
	};

	// depth first search for node with data (recursive)
	struct Node* findNode(Node* root, T* data) {
		for (int i = 0; i < root->connections.size(); i++) {
			if (root->connections[i]->data == data) {
				return root->connections[i];
			}

			Node* attempt = findNode(root->connections[i], data);
			if (attempt != nullptr) {
				return attempt;
			}
		}

		return nullptr;
	}

	struct Node* newNode(T* data, vector<Node*> conn = vector<Node*>()) {
		Node* node = new Node();

		node->id = size++;
		node->data = data;

		// link both this node and their connections
		for (int i = 0; i < conn.size(); i++) {
			node->connections.push_back(conn[i]);

			conn[i]->connections.push_back(node);
		}

		return node;
	}

	Node* rootNode;

public:
	struct Node;

	// You must initialize with the first Node data
	Graph() {
		rootNode = nullptr;
	}

	Graph(T* data) {
		rootNode = newNode(data);
	}

	// add a node based on the address of the parent node. Each number corresponds to the number of children in each branch.
	Node* addNode(T* data, vector<T*> connData) {
		// list of connected nodes
		vector<Node*> conn;

		for (int i = 0; i < connData.size(); i++) {
			Node* temp = findNode(rootNode, connData[i]);

			if (temp != nullptr) {
				conn.push_back(temp);
			}
		}

		return newNode(data, conn);
	}

	Node* addNode(T* data, vector<Node*> conn) {
		return newNode(data, conn);
	}
};

#endif