/* Computer Graphics Project
* Author: Castillo Turrubiartes Eunice Saraí
* Created: August 2024 - present
*/

#include <GL/freeglut.h>
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include "Object3D.h"
#include "Face.h"
#include "Matrix.h"

using namespace std;
GLFWwindow* window;
vector<Object3D> objects3d;

//Variables para calcular FPS
int frame_count = 0, current_time = 0, previous_time = 0;
float fps = 0.0f;

//Variables movimiento con teclado
float cam_pos_x = 0.0f, cam_pos_y = 0.0f, cam_pos_z = 5.0f;
float cam_cen_x = 0.0f, cam_cen_y = 0.0f, cam_cen_z = 0.0f;
//Variables movimiento mouse
float angle_x = 0.0f, angle_y = 0.0f;
double last_mouse_x = 0.0, last_mouse_y = 0.0;
bool is_dragging = false; //Controlar si se está arrastrando el mouse

//Función movimiento mouse
void mouseMotion(/*GLFWwindow* window,double x, double y*/) {
	double current_mouse_x, current_mouse_y;
	//Obtain the current mouse position
	glfwGetCursorPos(window, &current_mouse_x, &current_mouse_y);

	if (is_dragging) {
		//Calculate mouse movement
		double dx = current_mouse_x - last_mouse_x;
		double dy = current_mouse_y - last_mouse_y;

		angle_x += dx * 0.1f; //Sensibilidad rotación en x
		angle_y += dy * 0.1f; //Sensibilidad rotación en y

		//Limitar el ángulo de rotación vertical para evitar que la cámara se voltee
		if (angle_y > 89.0f) angle_y = 89.0f;
		if (angle_y < -89.0f) angle_y = -89.0f;
	}

	//Actualizar la última posición del mouse
	last_mouse_x = current_mouse_x;
	last_mouse_y = current_mouse_y;
}

//Función para manejar el clic del mouse
void mouseButton(/*GLFWwindow* window,*/) {
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
		is_dragging = true;
	}
	else {
		is_dragging = false;
	}
}

//Función movimiento teclado
void handleKeypress() {
	//Movimiento de la vista de la cámara
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cam_pos_x += 0.2f / 100;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cam_pos_y += 0.2f / 100;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cam_pos_z += 0.2f / 100;
	if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) cam_pos_x -= 0.2f / 100;
	if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) cam_pos_y -= 0.2f / 100;
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) cam_pos_z -= 0.2f / 100;
	//Movimiento de la posición de la cámara
	if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) cam_cen_x += 0.2f / 100;
	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) cam_cen_y += 0.2f / 100;
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) cam_cen_z += 0.2f / 100;
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) cam_cen_x -= 0.2f / 100;
	if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) cam_cen_y -= 0.2f / 100;
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) cam_cen_z -= 0.2f / 100;

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window, true);
}

void printText(float x, float y, const char* text) {
	glRasterPos2f(x, y); //Set the position for text drawing
	for (const char* c = text; *c != '\0'; ++c)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}

//Draw FPS
void drawFPS() {
	//Switch to orthographic projection
	glMatrixMode(GL_PROJECTION);
	glPushMatrix(); //Save the current projection matrix
	glLoadIdentity();

	//Set up an orthographic projection (2D mode)
	gluOrtho2D(0, 800, 0, 600);

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix(); //Save the current modelview matrix
	glLoadIdentity();

	//Disable depth test so the text is not affected by the 3D depth
	glDisable(GL_DEPTH_TEST);

	//Set text color to white
	glColor3f(1.0f, 1.0f, 1.0f);

	//Prepare the FPS string
	char buffer[50];
	snprintf(buffer, sizeof(buffer), "FPS: %.2f", fps);
	printText(10, 580, buffer); //Draw FPS at the top-left corner

	//Restore depth testing
	glEnable(GL_DEPTH_TEST);
	//Restore the previous modelview matrix
	glPopMatrix();
	//Restore the previous projection matrix
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	//Switch back to modelview
	glMatrixMode(GL_MODELVIEW);
}

//Calculate FPS
void calculateFPS() {
	//Get the number of milliseconds since glutInit called
	current_time = glfwGetTime();
	//Increase frame count
	frame_count++;
	//Calculate time passed
	int time_interval = current_time - previous_time;

	if (time_interval >= 1.0) {
		fps = frame_count / time_interval;
		previous_time = current_time;
		frame_count = 0;
	}
}

bool loadOBJ(const std::string& path) {
	std::ifstream file(path);
	if (!file.is_open()) {
		std::cerr << "Error opening the file: " << path << std::endl;
		return false;
	}
	Object3D obj;
	std::string line;
	while (std::getline(file, line)) {
		std::istringstream ss(line);
		std::string token;
		ss >> token;

		if (token == "v") {
			Vertex vertex;
			ss >> vertex.x >> vertex.y >> vertex.z;

			//Asignar colores aleatorios
			vertex.r = static_cast<float>(rand()) / RAND_MAX;
			vertex.g = static_cast<float>(rand()) / RAND_MAX;
			vertex.b = static_cast<float>(rand()) / RAND_MAX;

			obj.vertices.push_back(vertex);
		}
		else if (token == "f") {
			Face face;
			ss >> face.v1 >> face.v2 >> face.v3;
			//Los indices en .obj comienzan desde 1, restamos 1 para comenzar en 0
			face.v1--;
			face.v2--;
			face.v3--;
			obj.faces.push_back(face);
		}
	}
	objects3d.push_back(obj);
	file.close();
	return true;
}

void display(void)
{
	/*  clear all pixels  */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //está borra la pantalla cada frame
	//color de borrado y el color del buffer

	//Color de los vértices, openGL degrada el color de los vértices por default por eso las caras tienen color
	//glColor3f(1.0f, 0.0f, 0.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glRotatef(angle_y, 1.0f, 0.0f, 0.0f); //Rotación vertical
	glRotatef(angle_x, 0.0f, 1.0f, 0.0f); //Rotación horizontal
	gluLookAt(cam_pos_x, cam_pos_y, cam_pos_z, cam_cen_x, cam_cen_y, cam_cen_z, 0.0, 1.0, 0.0);

	//Dibujo del objeto
	glBegin(GL_TRIANGLES);
	//Para empezar a dibujar se debe mandar a llamar la función glBegin
	for (int o = 0; o < objects3d.size(); o++) {
		for (int c = 0; c < objects3d[o].faces.size(); c++) {
			int index = objects3d[o].faces[c].v1;
			glColor3f(objects3d[o].vertices[index].r, objects3d[o].vertices[index].g, objects3d[o].vertices[index].b);
			glVertex3f(objects3d[o].vertices[index].x,
				objects3d[o].vertices[index].y,
				objects3d[o].vertices[index].z);

			index = objects3d[o].faces[c].v2;
			glColor3f(objects3d[o].vertices[index].r, objects3d[o].vertices[index].g, objects3d[o].vertices[index].b);
			glVertex3f(objects3d[o].vertices[index].x,
				objects3d[o].vertices[index].y,
				objects3d[o].vertices[index].z);

			index = objects3d[o].faces[c].v3;
			glColor3f(objects3d[o].vertices[index].r, objects3d[o].vertices[index].g, objects3d[o].vertices[index].b);
			glVertex3f(objects3d[o].vertices[index].x,
				objects3d[o].vertices[index].y,
				objects3d[o].vertices[index].z);
		}
	}
	glEnd();

	calculateFPS();
	drawFPS(); //Draw FPS

	glfwSwapBuffers(window); //se dibuja en un buffer de memoria atrás y cuando se deba cambiar de imagen, se cambia los buffers
	glFlush(); //se eliminan las cosas que se deban eliminar, porque ya se terminó de dibujar
}

void init(void)
{
	/*  select clearing (background) color       */
	glClearColor(0.0, 0.0, 0.0, 0.0); //establece el color del borrado

	/*  initialize viewing values  */
	glMatrixMode(GL_PROJECTION); //selecciona que tipo de matriz estamos trabajando
	glLoadIdentity(); //debe cargar la matriz identidad

	//glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
	//gluOrtho2D(0.0, 800, 0.0, 600); //Set the 2D orthographic projection

	gluPerspective(45.0, 800.0 / 600.0, 0.1, 100.0); //es la matriz de proyeccion
	//ángulo de vista, aspecto donde va a dibujar los vértices, setANear, setAFar
	//en 2D se llama proyeccion ortogonal y en 3D es proyeccion en perspectiva

	glMatrixMode(GL_MODELVIEW); //selecciona que tipo de matriz estamos trabajando
	glLoadIdentity(); //debe cargar la matriz identidad

	//0.0f, 0.0, 5.0f
	gluLookAt(cam_pos_x, cam_pos_y, cam_pos_z, cam_cen_x, cam_cen_y, cam_cen_z, 0.0, 1.0, 0.0);
	//se compone de tres vectores, ojo (posicion donde esta la camara), centro (hacia donde ve la camara) y vector hacia arriba (es el eje hacia arriba: y = 1 | -1)

	glEnable(GL_DEPTH_TEST); //el algoritmo del pintor, verifica la profundidad para que al hacer los modelos se hagan correctamente
}

/*
 *  Declare initial window size, position, and display mode
 *  (single buffer and RGBA).  Open window with "hello"
 *  in its title bar.  Call initialization routines.
 *  Register callback function to display graphics.
 *  Enter main loop and process events.
 */
int main(int argc, char** argv)
{
	glutInit(&argc, argv); //Inicializa OpenGL
	//glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH); //Dibuja, nos indica que usemos RGB, y el último es un buffer de profundidad
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	//Glut inicializa ventanas, teclado, etc.
	//glutInitWindowSize(800, 600); //Tamaño de la ventana
	//glutInitWindowPosition(100, 100); //Posición inicial de la ventana
	//glutCreateWindow("3D Model"); //Crea la ventana, podemos poner cualquier nombre a la ventana

	//Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(800, 600, "3D Model", NULL, NULL);
	if (!window) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	//Make the window's context current
	glfwMakeContextCurrent(window);
	//glfwSetKeyCallback(window, handleKeypress); //Movimiento usando las teclas
	//glfwSetCursorPosCallback(window, mouseMotion); //Movimiento del mouse con botón presionado
	//glfwSetMouseButtonCallback(window, mouseButton); //Detecta clics del mouse
	init(); //Initialize OpenGL settings

	//Cargar el modelo .obj
	if (!loadOBJ("C:\\Users\\Saraí\\source\\repos\\Project1\\cubo.obj")) {
		return -1;
	}
	if (!loadOBJ("C:\\Users\\Saraí\\source\\repos\\Project1\\esfera.obj")) {
		return -1;
	}
	if (!loadOBJ("C:\\Users\\Saraí\\source\\repos\\Project1\\capsula.obj")) {
		return -1;
	}

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
		double initial_time = glfwGetTime();
		display();
		mouseMotion();
		mouseButton();
		handleKeypress();
		glfwPollEvents();
	}

	//glutDisplayFunc(display); //Se debe registrar la función de dibujado
	//glutIdleFunc(idle); //For continuous updates (FPS)
	//glutIdleFunc(display); //Mientras el programa no está "haciendo algo"/inactivo se manda llamar la función para que este dibujando todo el tiempo o redibujando
	//glutMainLoop(); //Esta función tiene un ciclo que se detiene hasta que se cierre la aplicación y manda llamar dentro del ciclo la función display() para estar dibujando

	//Clean up GLFW
	glfwTerminate();

	return 0;   /* ISO C requires main to return int. */
}