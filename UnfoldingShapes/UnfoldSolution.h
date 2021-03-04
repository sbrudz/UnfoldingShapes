#ifndef UNFOLDSOLUTION_H
#define UNFOLDSOLUTION_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <vector>

#include "Face.h"

using namespace std;

class UnfoldSolution {
public:
	int size;

	struct Node {
		int id;
		Face* data;

		Node* parent;
		std::vector<Node*> children;
	};

	// depth first search for node with data (recursive)
	struct Node* findNode(Node* root, Face* data) {
		for (int i = 0; i < root->children.size(); i++) {
			if (root->children[i]->data == data) {
				return root->children[i];
			}

			Node* attempt = findNode(root->children[i], data);
			if (attempt != nullptr) {
				return attempt;
			}
		}

		return nullptr;
	}

	struct Node* newNode(Face* data, Node* parent = NULL) {
		Node* node = new Node();

		node->id = size++;
		node->data = data;
		node->parent = parent;
		node->children = vector<Node*>();

		if (parent != NULL) {
			parent->children.push_back(node);
		}

		return node;
	}

	Node* rootNode;

	UnfoldSolution(Face* base) {
		rootNode = newNode(base, NULL);
	}

	// add a node based on the address of the parent node. Each number corresponds to the number of children in each branch.
	bool addNode(Face* data, vector<int> address) {
		Node* index = rootNode;

		for (int i = 0; i < address.size(); i++) {
			if (address[i] < index->children.size()) {
				index = index->children[address[i]];
			}
			else {
				return false;
			}
		}

		index->children.push_back(newNode(data, index));

		return true;
	}

	Node* addNode(Face* data, Node* parent) {
		parent->children.push_back(newNode(data, parent));

		return parent->children[parent->children.size() - 1];
	}
};

#endif
