#ifndef FACE_H
#define FACE_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <shader.h>

#include <string>
#include <vector>

#include "Mesh.h"

float getTriangleArea(glm::vec3 a, glm::vec3 b, glm::vec3 c);

// graphics method for each face
class Face {
public:
	Mesh* mesh;

	glm::vec3 rot;

	struct Axis {
		glm::vec3 axis;

		float originalAngle;
		
		Axis(glm::vec3 axis) {
			this->axis = axis;
		}

		glm::vec3 rotateAbout(glm::vec3 point, float angle) {
			glm::mat4 rotationMat(1);

			rotationMat = glm::rotate(rotationMat, angle, axis);

			return glm::vec3(rotationMat * glm::vec4(point, 1.0));
		}

		bool operator==(Axis a) {
			if (axis == a.axis) 
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
		// find all vertices that only have two or less indices marked of them and then connect them within the triangle.
		vector<unsigned int> validVertices;

		for (int i = 0; i < mesh->vertices.size(); i++) {
			int count = 0;
			
			for (int j = 0; j < mesh->indices.size(); j++) {
				if (i == mesh->indices[j]) {
					count++;
				}
			}

			if (count != 0 && count <= 2) {
				validVertices.push_back(i);
			}
		}

		// continue where left off 3/3/21
	}

	float getArea() {
		float area = 0;

		for (int i = 0; i < mesh->indices.size(); i += 3) {
			area += getTriangleArea(mesh->vertices[mesh->indices[i]].Position, mesh->vertices[mesh->indices[i+1]].Position, mesh->vertices[mesh->indices[i+2]].Position);
		}
		
		return area;
	}
};

float getTriangleArea(glm::vec3 a, glm::vec3 b, glm::vec3 c) {
	glm::vec3 ortho = glm::cross(a - b, a - c);

	return sqrt(ortho.x*ortho.x + ortho.y*ortho.y + ortho.z*ortho.z) / 2;
}

#endif
