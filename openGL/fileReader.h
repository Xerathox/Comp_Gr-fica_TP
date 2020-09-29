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

class CProgramaShaders {
	GLuint idPrograma;
public:
	CProgramaShaders(string rutaShaderVertice, string rutaShaderFragmento) {
		//Variables para leer los archivos de código
		string strCodigoShaderVertice;
		string strCodigoShaderFragmento;
		ifstream pArchivoShaderVertice;
		ifstream pArchivoShaderFragmento;
		//Mostramos excepciones en caso haya
		pArchivoShaderVertice.exceptions(ifstream::failbit | ifstream::badbit);
		pArchivoShaderFragmento.exceptions(ifstream::failbit | ifstream::badbit);
		try {
			//Abriendo los archivos de código de los shader
			pArchivoShaderVertice.open(rutaShaderVertice);
			pArchivoShaderFragmento.open(rutaShaderFragmento);
			//Leyendo la información de los archivos
			stringstream lectorShaderVertice, lectorShaderFragmento;
			lectorShaderVertice << pArchivoShaderVertice.rdbuf();
			lectorShaderFragmento << pArchivoShaderFragmento.rdbuf();
			//Cerrando los archivos
			pArchivoShaderVertice.close();
			pArchivoShaderFragmento.close();
			//Pasando la información leida a string
			strCodigoShaderVertice = lectorShaderVertice.str();
			strCodigoShaderFragmento = lectorShaderFragmento.str();
		}
		catch (ifstream::failure) {
			cout << "ERROR: Los archivos no pudieron ser leidos correctamente.\n";
		}
		const char* codigoShaderVertice = strCodigoShaderVertice.c_str();
		const char* codigoShaderFragmento = strCodigoShaderFragmento.c_str();
		//Asociando y compilando los códigos de los shader
		GLuint idShaderVertice, idShaderFragmento;
		//Shader de Vértice
		idShaderVertice = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(idShaderVertice, 1, &codigoShaderVertice, NULL);
		glCompileShader(idShaderVertice);
		verificarErrores(idShaderVertice, "Vértice");
		//Shader de Fragmento
		idShaderFragmento = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(idShaderFragmento, 1, &codigoShaderFragmento, NULL);
		glCompileShader(idShaderFragmento);
		verificarErrores(idShaderVertice, "Fragmento");
		//Programa de Shaders
		this->idPrograma = glCreateProgram();
		glAttachShader(this->idPrograma, idShaderVertice);
		glAttachShader(this->idPrograma, idShaderFragmento);
		glLinkProgram(this->idPrograma);
		//Ahora ya podemos eliminar los programas de los shaders
		glDeleteShader(idShaderVertice);
		glDeleteShader(idShaderFragmento);
	}
	~CProgramaShaders() {
		glDeleteProgram(this->idPrograma);
	}
	void usar() const {
		glUseProgram(this->idPrograma);
	}
	void setVec3(const string& nombre, float x, float y, float z) const {
		glUniform3f(glGetUniformLocation(this->idPrograma, nombre.c_str()), x, y, z);
	}
	void setVec3(const string& nombre, const glm::vec3& valor) const {
		glUniform3fv(glGetUniformLocation(this->idPrograma, nombre.c_str()), 1, &valor[0]);
	}
	void setMat4(const string& nombre, const glm::mat4& mat) const {
		glUniformMatrix4fv(glGetUniformLocation(this->idPrograma, nombre.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
private:
	void verificarErrores(GLuint identificador, string tipo) {
		GLint ok;
		GLchar log[1024];
		if (tipo == "Programa") {
			glGetProgramiv(this->idPrograma, GL_LINK_STATUS, &ok);
			if (!ok) {
				glGetProgramInfoLog(this->idPrograma, 1024, NULL, log);
				cout << "Error de enlace con el programa: " << log << "\n";
			}
		}
		else {
			glGetShaderiv(identificador, GL_COMPILE_STATUS, &ok);
			if (!ok) {
				glGetShaderInfoLog(identificador, 1024, nullptr, log);
				cout << "Error de compilación con el Shader de " << tipo << ": " << log << "\n";
			}
		}
	}
};

typedef struct Superficie {
	GLfloat* vertices;
	GLuint* indices;
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
