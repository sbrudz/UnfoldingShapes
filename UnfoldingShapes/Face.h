#ifndef FACE_H
#define FACE_H

#include <glad/glad.h> // holds all OpenGL type declarations

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/closest_point.hpp>

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
		const float marginOfError = 0.0001f;

		// stores the axis with a point for refrence and the vector of the line
		glm::vec3 point;
		glm::vec3 line;

		float originalAngle;
		
		Axis(glm::vec3 p1, glm::vec3 p2) {
			// normalize and get abs to standardize it
			line = glm::abs(glm::normalize(p1 - p2));

			point = glm::closestPointOnLine(glm::vec3(0), p1 + line*-1000000.0f, p2 + line*1000000.0f);
		}

		glm::vec3 rotateAbout(glm::vec3 p, float angle) {
			glm::mat4 rotationMat(1);

			rotationMat = glm::rotate(rotationMat, angle, line);

			// translate the point to its relative position on the axis and then back to is actual position.
			return glm::vec3(rotationMat * glm::vec4(p-point, 1.0)) + point;
		}

		// transform this axis based on another axis
		void rotateAxisAbout(Axis* axis, float angle) {
			//std::cout << "1 " << glm::to_string(line) << " " << glm::to_string(point) << std::endl;
			glm::vec3 point1 = line * -1.0f + point;
			glm::vec3 point2 = line * 1.0f + point;

			point1 = axis->rotateAbout(point1, angle);
			point2 = axis->rotateAbout(point2, angle);

			// normalize and get abs to standardize it
			line = glm::abs(glm::normalize(point1 - point2));

			point = glm::closestPointOnLine(glm::vec3(0), point1 + line * -1000000.0f, point2 + line * 1000000.0f);

			//std::cout << "2 " << glm::to_string(line) << " " << glm::to_string(point) << std::endl;
		}

		float orientedAngle(glm::vec3 p1, glm::vec3 p2) {
			p1 = p1 - point;
			p2 = p2 - point;

			return glm::orientedAngle(p1, p2, line);
		}

		// check if the axis contains a point
		bool hasPoint(glm::vec3 p) {
			//std::cout << glm::to_string(line) << " " << glm::to_string(point) << std::endl;
			glm::vec3 pointOnLine = glm::closestPointOnLine(p, point + line * -1000000.0f, point + line * 1000000.0f);

			if (glm::distance(pointOnLine, p) <= marginOfError) {
				return true;
			}

			return false;
		}

		bool operator==(Axis a) {
			if (glm::distance(point, a.point) <= marginOfError && glm::distance(line, a.line) <= marginOfError)
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
