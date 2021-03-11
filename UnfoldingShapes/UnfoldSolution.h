#ifndef UNFOLDSOLUTION_H
#define UNFOLDSOLUTION_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <vector>

#include "Face.h"
#include "Graph.h"

using namespace std;

class UnfoldSolution {
public:
	Graph<Face> solution;

	UnfoldSolution(Graph<Face> solution) {
		this->solution = solution;
	}


};

#endif
