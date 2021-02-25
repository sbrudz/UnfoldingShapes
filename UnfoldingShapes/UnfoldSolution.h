#ifndef UNFOLDSOLUTION_H
#define UNFOLDSOLUTION_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <vector>

using namespace std;

class UnfoldSolution {
	int size;

	struct Node {
		int id;
		void* data;

		Node* parent;
		std::vector<Node*> children;
	};

	struct Node* newNode(void* data, Node* parent = NULL) {
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

public:
	UnfoldSolution() {
		rootNode = newNode(NULL);
	}

	// add a node based on the address of the parent node. Each number corresponds to the number of children in each branch.
	bool addNode(void* data, vector<int> address) {
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
};

#endif
