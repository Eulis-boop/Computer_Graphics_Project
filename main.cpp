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
#include "Vec3.h"

using namespace std;
GLFWwindow* window;
vector<Object3D> objects3d;

//Variables para calcular FPS
int frame_count = 0, current_time = 0, previous_time = 0;
float fps = 0.0f;

//Variables movimiento con teclado
float cam_pos_x = 0.0f, cam_pos_y = 0.0f, cam_pos_z = 30.0f;
float cam_cen_x = 0.0f, cam_cen_y = 0.0f, cam_cen_z = 0.0f;
//Variables movimiento mouse
float angle_x = 0.0f, angle_y = 0.0f;
double last_mouse_x = 0.0, last_mouse_y = 0.0;
bool is_dragging = false; //Controlar si se está arrastrando el mouse

// Variables para curva de Bezier
float radius = 2.5;

//Implementación función LookAt
float** lookAt(const Vec3& eye, const Vec3& target, const Vec3& up) {
	//Calculate the forward vector
	Vec3 forward = (target - eye).normalize();
	//Calculate the right vector
	Vec3 right = up.cross(forward).normalize();
	//Calculate the up vector
	Vec3 up_corrected = forward.cross(right).normalize();

	//Build the lookAt matrix
	float** lookAt_matrix = new float* [4];
	for (int i = 0; i < 4; i++)
		lookAt_matrix[i] = new float[4]; // Allocate each row

	// Primera fila
	lookAt_matrix[0][0] = right.x;
	lookAt_matrix[0][1] = right.y;
	lookAt_matrix[0][2] = right.z;
	lookAt_matrix[0][3] = -right.dot_negative(eye);

	// Segunda fila
	lookAt_matrix[1][0] = up_corrected.x;
	lookAt_matrix[1][1] = up_corrected.y;
	lookAt_matrix[1][2] = up_corrected.z;
	lookAt_matrix[1][3] = -up_corrected.dot_negative(eye);

	// Tercera fila
	lookAt_matrix[2][0] = forward.x;
	lookAt_matrix[2][1] = forward.y;
	lookAt_matrix[2][2] = forward.z;
	lookAt_matrix[2][3] = -forward.dot_negative(eye);

	// Cuarta fila
	lookAt_matrix[3][0] = 0.0f;
	lookAt_matrix[3][1] = 0.0f;
	lookAt_matrix[3][2] = 0.0f;
	lookAt_matrix[3][3] = 1.0f;

	return lookAt_matrix;
}


//Curva de Bézier
Vertex Bezier(Vertex p1, Vertex p2, Vertex p3, Vertex p4, float t) {
	Vertex res;
	res.x = powf(1 - t, 3) * p1.x + 3 * powf(1 - t, 2) * t * p2.x + 3 * (1 - t) * powf(t, 2) * p3.x + powf(t, 3) * p4.x;
	res.y = powf(1 - t, 3) * p1.y + 3 * powf(1 - t, 2) * t * p2.y + 3 * (1 - t) * powf(t, 2) * p3.y + powf(t, 3) * p4.y;
	res.z = powf(1 - t, 3) * p1.z + 3 * powf(1 - t, 2) * t * p2.z + 3 * (1 - t) * powf(t, 2) * p3.z + powf(t, 3) * p4.z;
	return res;
}

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

bool loadOBJ(const string& path) {
	ifstream file(path);
	if (!file.is_open()) {
		cerr << "Error opening the file: " << path << endl;
		return false;
	}
	Object3D obj;
	string line;
	while (getline(file, line)) {
		istringstream ss(line);
		string token;
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
			obj.caras.push_back(face);
		}
	}
	objects3d.push_back(obj);
	file.close();
	return true;
}	

float rot_angle_x = 45.0f;
float rot_angle_z = 60.0f;
void display(void)
{
	/*  clear all pixels  */
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //está borra la pantalla cada frame
	//color de borrado y el color del buffer

	//Color de los vértices, openGL degrada el color de los vértices por default por eso las faces tienen color
	//glColor3f(1.0f, 0.0f, 0.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//glRotatef(angle_y, 1.0f, 0.0f, 0.0f); //Rotación vertical
	//glRotatef(angle_x, 0.0f, 1.0f, 0.0f); //Rotación horizontal
	//gluLookAt(cam_pos_x, cam_pos_y, cam_pos_z, cam_cen_x, cam_cen_y, cam_cen_z, 0.0, 1.0, 0.0);
	Vec3 eye = {cam_pos_x, cam_pos_y, cam_pos_z};
	Vec3 target = { cam_cen_x, cam_cen_y, cam_cen_z };
	Vec3 up = { 0.0f, 1.0f, 0.0f };
	float** view_matrix = lookAt(eye, target, up);
	
	float** mt, ** mr, ** mm, ** to, ** tpo;

	// Dibujo del objeto
	glBegin(GL_TRIANGLES);
	// Para empezar a dibujar se debe mandar a llamar la funci�n glBegin
	for (int o = 0; o < objects3d.size(); o++) {
		Vertex b = Bezier(objects3d[o].modelo.p[0], objects3d[o].modelo.p[1], objects3d[o].modelo.p[2], objects3d[o].modelo.p[3], objects3d[o].t);
		mt = objects3d[o].modelo.trasMatrix(b.x - objects3d[o].center.x, b.y - objects3d[o].center.y, b.z - objects3d[o].center.z);
		
		if (o == 0) {
			objects3d[o].modelo.ry += 1.0f;
			mr = objects3d[o].modelo.rotMatrix(objects3d[o].center);
		} else {
			// Matriz de traslación para mover el objeto 1 hacia el centro del objeto 0
			to = objects3d[o].modelo.trasMatrix(-objects3d[0].center.x, -objects3d[0].center.y, -objects3d[0].center.z);

			// Rotaciones en cada eje alrededor del objeto 0
			float** rotationX = objects3d[o].modelo.rxMatrix(rot_angle_x);
			//float** rotationZ = objects3d[o].modelo.rzMatrix(rot_angle_z);
			rot_angle_x += 4.0f;
			// rot_angle_z += 4.0f;

			// Traslada el objeto de regreso desde el origen relativo al objeto 0
			tpo = objects3d[o].modelo.trasMatrix(objects3d[0].center.x, objects3d[0].center.y, objects3d[0].center.z);

			// Orden correcto: traslación al origen, rotación, traslación de regreso
			mr = objects3d[o].modelo.multMatrix(to, rotationX);
			mr = objects3d[o].modelo.multMatrix(mr, tpo);
		}
		mm = objects3d[o].modelo.multMatrix(mt, mr);
		mm = objects3d[o].modelo.multMatrix(view_matrix, mm);

		for (int c = 0; c < objects3d[o].caras.size(); c++) {
			Vertex v1 = objects3d[o].vertices[objects3d[o].caras[c].v1];
			Vertex v2 = objects3d[o].vertices[objects3d[o].caras[c].v2];
			Vertex v3 = objects3d[o].vertices[objects3d[o].caras[c].v3];

			Vertex vt1 = objects3d[o].modelo.resVector(mm, v1);
			glColor3f(vt1.r, vt1.g, vt1.b);
			glVertex3f(vt1.x, vt1.y, vt1.z);

			Vertex vt2 = objects3d[o].modelo.resVector(mm, v2);
			glColor3f(vt2.r, vt2.g, vt2.b);
			glVertex3f(vt2.x, vt2.y, vt2.z);

			Vertex vt3 = objects3d[o].modelo.resVector(mm, v3);
			glColor3f(vt3.r, vt3.g, vt3.b);
			glVertex3f(vt3.x, vt3.y, vt3.z);
		}
		objects3d[o].t += objects3d[o].incremento;
		if (objects3d[o].t >= 1) objects3d[o].t = 0;
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

	glEnable(GL_DEPTH_TEST); //el algoritmo del pintor, verifica la profundidad para que al hacer los models se hagan correctamente
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
		cerr << "Failed to initialize GLFW" << endl;
		return -1;
	}

	//Create a windowed mode window and its OpenGL context
	window = glfwCreateWindow(800, 600, "3D Model", NULL, NULL);
	if (!window) {
		cerr << "Failed to create GLFW window" << endl;
		glfwTerminate();
		return -1;
	}

	//Make the window's context current
	glfwMakeContextCurrent(window);
	init(); //Initialize OpenGL settings
	
	glfwSwapInterval(1);

	//Cargar el modelo .obj
	if (!loadOBJ("C:\\Users\\Saraí\\source\\repos\\Project1\\cubo.obj")) {
		return -1;
	}
	if (!loadOBJ("C:\\Users\\Saraí\\source\\repos\\Project1\\esfera.obj")) {
		return -1;
	}
	/*if (!loadOBJ("C:\\Users\\Saraí\\source\\repos\\Project1\\capsula.obj")) {
		return -1;
	}*/

	objects3d[0].modelo.changeP(0, -6.0f, -2.5f, 1.0f);
	objects3d[0].modelo.changeP(1, -6.0f, 6.0f, 1.0f);
	objects3d[0].modelo.changeP(2, 5.0f, 6.0f, 1.0f);
	objects3d[0].modelo.changeP(3, 5.0f, -2.5f, 1.0f);

	objects3d[1].modelo.changeP(0, -6.0f, -2.5f, 1.0f);
	objects3d[1].modelo.changeP(1, -6.0f, 6.0f, 1.0f);
	objects3d[1].modelo.changeP(2, 5.0f, 6.0f, 1.0f);
	objects3d[1].modelo.changeP(3, 5.0f, -2.5f, 1.0f);

	objects3d[1].modelo.changeRot(0.0f, 45.5f, 0.0f);
	objects3d[0].modelo.changeRot(0.0f, 0.0f, 0.0f);

	while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && glfwWindowShouldClose(window) == 0) {
		double initial_time = glfwGetTime();
		display();
		mouseMotion();
		mouseButton();
		handleKeypress();
		glfwPollEvents();
	}

	//Clean up GLFW
	glfwTerminate();

	return 0;   /* ISO C requires main to return int. */
}