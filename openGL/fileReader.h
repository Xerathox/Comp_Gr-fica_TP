#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cmath>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

using namespace std;

typedef struct Superficie {
	GLfloat* vertices;
	GLuint* indices;
	//GLfloat* texturas;
	int verticesTotal;
	int indicesTotal;
	string tipo; // 2D 3D 3DC 3DT
};

void readOFF(string ruta, Superficie* M) {
	ifstream file(ruta);
	string line;
	bool first = true;
	int numVertices, numCaras, numAristas;
	float a, b, c, d, e, f, g, h;

	if (file.is_open()) {
		getline(file, line); // OFF
		file >> numVertices >> numCaras >> numAristas;

		for (int i = 0; i < numVertices; i++) {
			if (first) {
				file.ignore(); // dummy
				getline(file, line);
				stringstream ss(line);

				vector<float> auxVec;
				float auxFloat;
				while (ss >> auxFloat) {
					auxVec.push_back(auxFloat);
				}

				if (auxVec.size() == 3) M->tipo = "3D";
				if (auxVec.size() == 6) M->tipo = "3DC";
				if (auxVec.size() == 8) M->tipo = "3DT";

				M->verticesTotal = numVertices * auxVec.size();
				M->vertices = new float[M->verticesTotal];
				for (int j = 0; j < auxVec.size(); j++) M->vertices[j] = auxVec[j];

				first = false;
			}
			else {
				if (M->tipo == "3D") {
					file >> a >> b >> c;
					M->vertices[3 * i] = a;
					M->vertices[3 * i + 1] = b;
					M->vertices[3 * i + 2] = c;
				}
				if (M->tipo == "3DC") {
					file >> a >> b >> c >> d >> e >> f;
					M->vertices[6 * i] = a;
					M->vertices[6 * i + 1] = b;
					M->vertices[6 * i + 2] = c;
					M->vertices[6 * i + 3] = d;
					M->vertices[6 * i + 4] = e;
					M->vertices[6 * i + 5] = f;
				}
				if (M->tipo == "3DT") {
					file >> a >> b >> c >> d >> e >> f >> g >> h;
					M->vertices[8 * i] = a;
					M->vertices[8 * i + 1] = b;
					M->vertices[8 * i + 2] = c;
					M->vertices[8 * i + 3] = d;
					M->vertices[8 * i + 4] = e;
					M->vertices[8 * i + 5] = f;
					M->vertices[8 * i + 6] = g;
					M->vertices[8 * i + 7] = h;
				}
			}
		}

		if (M->tipo == "3D" && numCaras == 0) {
			M->tipo = "2D";
		}

		if (M->tipo != "2D") {
			M->indicesTotal = numCaras * 3;
			M->indices = new unsigned int[M->indicesTotal];

			for (int i = 0; i < numCaras; i++) {
				file >> a >> b >> c >> d;
				M->indices[3 * i] = b;
				M->indices[3 * i + 1] = c;
				M->indices[3 * i + 2] = d;
			}
		}
	}
	file.close();
}