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
#include "fileReader.h"


void framebuffer_tamanho_callback(GLFWwindow* ventana, int ancho, int alto) {
	glViewport(0, 0, ancho, alto);
}
void procesarEntrada(GLFWwindow* ventana) {
	if (glfwGetKey(ventana, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(ventana, true);
}

const unsigned int ANCHO = 800;
const unsigned int ALTO = 600;

//Class programa shader

int main() {
	//Inicializar glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Creando la ventana
	GLFWwindow* ventana = glfwCreateWindow(ANCHO, ALTO, "Compu Grafica", NULL, NULL);
	if (ventana == NULL) {
		cout << "Problemas al crear la ventana\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(ventana); 
	glfwSetFramebufferSizeCallback(ventana, framebuffer_tamanho_callback);

	//Cargar Glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		cout << "Problemas al cargar GLAD\n";
		return -1;
	}

	//Definiendo la geometría de la figura en función de vértices
	Superficie modelo;
	readOFF("OFF/avion.off", &modelo);
	cout << modelo.tipo << endl;

	string ruta = "GLSL/codigo.vs";
	if (modelo.tipo == "3DC") ruta = "GLSL/codigoColor.vs";
	CProgramaShaders programa_shaders = CProgramaShaders(ruta, "GLSL/codigo.fs");

	//Enviando la geometría al GPU: Definiendo los buffers (Vertex Array Objects y Vertex Buffer Objects)
	unsigned int id_array_vertices, id_array_buffers, id_element_buffer;
	glGenVertexArrays(1, &id_array_vertices);
	glGenBuffers(1, &id_array_buffers);
	glGenBuffers(1, &id_element_buffer);

	//anexando los buffers para su uso en la tarjeta gráfica
	glBindVertexArray(id_array_vertices);

	//Anexando buffers y cargandolos con los datos
	//vertices
	glBindBuffer(GL_ARRAY_BUFFER, id_array_buffers);
	glBufferData(GL_ARRAY_BUFFER, modelo.verticesTotal * sizeof(GLfloat), modelo.vertices, GL_STATIC_DRAW);
	//indices
	if (modelo.tipo == "3D") {
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_element_buffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, modelo.indicesTotal * sizeof(GLuint), modelo.indices, GL_STATIC_DRAW);
	}
	
	//Indicando las especificaciones de los atributos
	if (modelo.tipo == "2D" || modelo.tipo == "3D") {
		//Atributo de posicion
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
	}
	if (modelo.tipo == "3DC") {
		//Atributo de posicion
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);
		//Atributo de color
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
	}
	if (modelo.tipo == "3DT") {
		//añadir texturas
		return -1;
	}

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	float theTime, sinTime, cosTime, x = 0.0, dx = 0.0025, y = 0.0, dy = 0.0035;
	float sizeOb = 0.5;
	while (!glfwWindowShouldClose(ventana)) {
		theTime = (float)glfwGetTime();
		sinTime = glm::sin(theTime);
		cosTime = glm::cos(theTime);

		procesarEntrada(ventana);
		glClearColor(0, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		programa_shaders.usar();
		glm::mat4 transformacion = glm::mat4(1.0);
		transformacion = glm::translate(transformacion, glm::vec3(0.0, 0.0, 0.0));
		transformacion = glm::scale(transformacion, glm::vec3(0.125, 0.125, 0.125));
		transformacion = glm::rotate(transformacion, theTime, glm::vec3(0.0, 0.5, 0.0));
		programa_shaders.setMat4("transformacion", transformacion);
		if (modelo.tipo != "3DC" && modelo.tipo != "3DT") {
			programa_shaders.setVec3("colores", glm::vec3(0.1, 0.2, 0.3));
		}
		
		glBindVertexArray(id_array_vertices);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id_element_buffer);
		if (modelo.tipo == "2D" || modelo.tipo == "3DC") {
			glDrawArrays(GL_TRIANGLES, 0, modelo.verticesTotal);
		}
		else {
			glDrawElements(GL_TRIANGLES, modelo.indicesTotal, GL_UNSIGNED_INT, 0);
		}

		glfwSwapBuffers(ventana);
		glfwPollEvents();
		
		//if (x > (1 - sizeOb) || x < (sizeOb - 1)) dx *= -1;
		//if (y > (1 - sizeOb) || y < (sizeOb - 1)) dy *= -1;

		if (x > (1 - sizeOb) + abs(sizeOb * cosTime) || x < (sizeOb - 1) - abs(sizeOb * cosTime)) dx *= -1;
		if (y > (1 - sizeOb) + abs(sizeOb * cosTime) || y < (sizeOb - 1) - abs(sizeOb * cosTime)) dy *= -1;

		x += dx;
		y += dy;
	}

	//Liberando memoria
	glDeleteVertexArrays(1, &id_array_vertices);
	glDeleteBuffers(1, &id_array_buffers);
	glfwTerminate();
	return 0;
}
