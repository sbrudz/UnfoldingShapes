#ifndef FACE_H
#define FACE_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <shader.h>

#include <string>
#include <vector>

#include "Mesh.h"

// graphics method for each face
class Face {
public:
	Mesh* mesh;

	glm::vec3 rot;

	struct Axis {
		glm::vec3 point1;
		glm::vec3 point2;
		
		Axis(glm::vec3 p1, glm::vec3 p2) {
			point1 = p1;
			point2 = p2;
		}

		glm::vec3 rotateAbout(glm::vec3 point) {
			return point;
		}

		bool operator==(Axis a) {
			if (point1 == a.point1 &&
				point2 == a.point2) 
			{
				return true;
			}

			return false;
		}
	};

	vector<Axis> axis;


	Face(Mesh* mesh) {
		this->mesh = mesh;

		initAxis();
	}

	void initAxis() {

	}
};

#endif
