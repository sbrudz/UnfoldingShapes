#ifndef SHAPE_H
#define SHAPE_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <vector>

// graphics tools
#include "Camera.h"
#include "Light.h"
#include "Asset.h"
#include "Model.h"
#include "Mesh.h"
#include "Skybox.h"
#include "TextManager.h"
#include "GraphicsEngine.h"

#include "Face.h"
#include "Graph.h"

class Shape {
public:
	Asset* asset;
	Model* model;
	
	vector<Face> faces;

	Graph<Face> faceMap;

	// inactive
	Shape() {
		asset = nullptr;
	}

	// init Shape by setting the asset and registering all of the faces.
	// Copies the model because we are manipulating the face and vertex info.
	Shape(string const &path, glm::vec3 pos = glm::vec3(0), glm::vec3 rot = glm::vec3(0), glm::vec3 scale = glm::vec3(1)) {
		this->model = new Model(path, 1);
		asset = new Asset(this->model, pos, rot, scale);

		initFaces();
	}

	void initFaces() {
		for (int i = 0; i < model->meshes.size(); i++) {
			faces.push_back(Face(&model->meshes[i]));
		}
		
		// Largest face is the base
		// temp (FIX LATER)
		faceMap = Graph<Face>(&faces[0]);

		// find adjacent faces and then add them to graph (if they have the same axis)
		for (int i = 0; i < faces.size(); i++) {
			vector<Face*> tempFaces = vector<Face*>();

			for (int j = 0; j < faces.size() && j != i; j++) {
				for (int g = 0; g < faces[i].axis.size(); g++) {
					for (int h = g+1; h < faces[j].axis.size(); h++) {
						if (faces[i].axis[g] == faces[j].axis[h]) {
							tempFaces.push_back(&faces[j]);
						}
					}
				}
			}

			faceMap.addNode(&faces[i], tempFaces);
		}
	}
};

#endif
