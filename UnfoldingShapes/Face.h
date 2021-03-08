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
		glm::vec3 point1;
		glm::vec3 point2;

		float originalAngle;
		
		Axis(glm::vec3 p1, glm::vec3 p2) {
			point1 = p1;
			point2 = p2;
		}

		glm::vec3 rotateAbout(glm::vec3 point, float angle) {
			glm::mat4 rotationMat(1);

			rotationMat = glm::rotate(rotationMat, angle, point2-point1);

			// translate the point to its relative position on the axis and then back to is actual position.
			return glm::vec3(rotationMat * glm::vec4(point-point1, 1.0)) + point1;
		}

		float orientedAngle(glm::vec3 p1, glm::vec3 p2) {
			p1 = p1 - point1;
			p2 = p2 - point1;

			return glm::orientedAngle(p1, p2, point2 - point1);
		}

		bool operator==(Axis a) {
			if ((point1 == a.point1 && point2 == a.point2) || (point1 == a.point2 && point2 == a.point1)) 
			{
				return true;
			}

			return false;
		}
	};

	vector<Axis*> axis;


	Face(Mesh* mesh) {
		this->mesh = mesh;

		initAxis();
	}

	void initAxis() {
		// find all vertices that only have two or less indices marked of them and then connect them within the triangle.
		vector<unsigned int> validVertices;

		// identify vertices that are part of 1 or 2 triangles
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

		// generate axis by matching indicies
		// match two of the found vertices in the same triangle
		for (int i = 0; i < validVertices.size(); i++) {
			for (int j = 0; j < validVertices.size() && j != i; j++) {
				for (int g = 0; g < mesh->indices.size(); g += 3) {
					int verticesInTriangle = 0;

					for (int h = g; h < g + 3; h++) {
						if (validVertices[i] == mesh->indices[h] || validVertices[j] == mesh->indices[h]) {
							verticesInTriangle++;
						}
					}

					if (verticesInTriangle == 2) {
						axis.push_back(new Axis(mesh->vertices[validVertices[i]].Position, mesh->vertices[validVertices[j]].Position));
					}
				}
			}
		}

		// delete both duplicates if found because this means that triangles were sharing an inside edge (we only want outside edges)
		vector<unsigned int> toRemove;

		for (int i = 0; i < axis.size(); i++) {
			bool collision = false;

			for (int j = i + 1; j < axis.size(); j++) {
				if (*axis[i] == *axis[j]) {
					toRemove.push_back(j);
					collision = true;
				}
			}

			if (collision) {
				toRemove.push_back(i);
			}
		}

		// apply removal
		// first sort all of them
		for (unsigned int i = 0; i < toRemove.size(); i++) {
			bool swapped = false;
			for (unsigned int j = 0; j < toRemove.size() - 1 - i; j++) {
				if (toRemove[j] > toRemove[j+1]) {
					int temp = toRemove[j];
					toRemove[j] = toRemove[j + 1];
					toRemove[j + 1] = temp;

					swapped = true;
				}
			}

			//exit search
			if (swapped == false) {
				break;
			}
		}

		// finally remove from list
		for (int i = toRemove.size() - 1; i >= 0; i--) {
			axis.erase(axis.begin() + toRemove[i]);
		}
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
