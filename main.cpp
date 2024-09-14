/* Computer Graphics Project
* Author: Castillo Turrubiartes Eunice Saraí
* Created: August 2024 - present
*/

#include <GL/freeglut.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
//#include <ncurses.h>
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
//#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#include "Object3D.h"
#include "Face.h"

using namespace std;

vector<Object3D> objects3d;

//Variables para calcular FPS
int frame_count = 0, current_time = 0, previous_time = 0;
float fps = 0.0f;

//Variables movimiento con teclado
float cam_pos_x = 0.0f, cam_pos_y = 0.0f, cam_pos_z = 5.0f;
float cam_cen_x = 0.0f, cam_cen_y = 0.0f, cam_cen_z = 0.0f;
//Variables movimiento mouse
float angle_x = 0.0f, angle_y = 0.0f;
int last_mouse_x = 0, last_mouse_y = 0;
bool is_dragging = false; //Controlar si se está arrastrando el mouse

//Función movimiento mouse
void mouseMotion(int x, int y) {
	if (is_dragging) {
		int dx = x - last_mouse_x;
		int dy = y - last_mouse_y;

		//Ajustar ángulos en función del movimiento del mouse
		angle_x += dx * 0.1f; //Sensibilidad rotación en x
		angle_y += dy * 0.1f; //Sensibilidad rotación en y

		//Limita el ángulo de rotación vertical para evitar que la cámara se voltee
		if (angle_y > 89.0f) angle_y = 89.0f;
		if (angle_y < -89.0f) angle_y = -89.0f;

		last_mouse_x = x;
		last_mouse_y = y;
	}
}
bool f = false;

//Función para manejar el clic del mouse
void mouseButton(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			is_dragging = true;
			last_mouse_x = x;
			last_mouse_y = y;
		}
		else {
			is_dragging = false;
		}
	}
}

//Función movimiento teclado
void handleKeypress(unsigned char key, int x, int y) {
	switch (key) {
		//Movimiento de la posición de la cámara
	case 'A': case 'a':
		cam_pos_x += 0.2f;
		break;
	case 'S': case 's':
		cam_pos_y += 0.2f;
		break;
	case 'D': case 'd':
		cam_pos_z += 0.2f;
		break;
	case 'Z': case 'z':
		cam_pos_x -= 0.2f;
		break;
	case 'X': case 'x':
		cam_pos_y -= 0.2f;
		break;
	case 'C': case 'c':
		cam_pos_z -= 0.2f;
		break;

		//Movimiento del centro (hacia dónde ve la cámara)
	case 'H': case 'h':
		cam_cen_x += 0.2f;
		break;
	case 'J': case 'j':
		cam_cen_y += 0.2f;
		break;
	case 'K': case 'k':
		cam_cen_z += 0.2f;
		break;
	case 'B': case 'b':
		cam_cen_x -= 0.2f;
		break;
	case 'N': case 'n':
		cam_cen_y -= 0.2f;
		break;
	case 'M': case 'm':
		cam_cen_z -= 0.2f;
		break;
	case 27: //Esc para salir
		exit(0);
		break;
	}
}

void printText(float x, float y, const char* text) {
	glRasterPos2f(x, y); //Set the position for text drawing
	for (const char* c = text; *c != '\0'; ++c)
		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
}

//Draw FPS
void drawFPS() {
	glColor3f(1.0f, 1.0f, 1.0f); //Set text color to white
	char buffer[50];
	snprintf(buffer, sizeof(buffer), "FPS: %.2f", fps);
	printText(10, 10, buffer); //Draw FPS at (10, 10)
}

//Calculate FPS
void calculateFPS() {
	//Increase frame count
	frame_count++;
	//Get the number of milliseconds since glutInit called
	current_time = glutGet(GLUT_ELAPSED_TIME);
	//Calculate time passed
	int time_interval = current_time - previous_time;

	if (time_interval > 1000) {
		//Calculate the number of frames per second
		fps = frame_count / (time_interval / 1000.0f);
		//Set time
		previous_time = current_time;
		//Reset frame count
		frame_count = 0;
	}
}

/*This function is called when OpenGL/GLUT is not working on something else.
	It's like the timers but time intervals are dependent on how busy the app is,
	instead of having a constant value set by the user.
*/
void idle(void) {
	//Calculate FPS
	calculateFPS();
	
	//Call display function (draw the current frame)
	glutPostRedisplay();
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

	drawFPS(); //Draw FPS

	glutSwapBuffers(); //se dibuja en un buffer de memoria atrás y cuando se deba cambiar de imagen, se cambia los buffers
	glFlush(); //se eliminan las cosas que se deban eliminar, porque ya se terminó de dibujar
}

void init(void)
{
	/*  select clearing (background) color       */
	glClearColor(0.0, 0.0, 0.0, 0.0); //establece el color del borrado

	/*  initialize viewing values  */
	glMatrixMode(GL_PROJECTION); //selecciona que tipo de matriz estamos trabajando
	glLoadIdentity(); //debe cargar la matriz identidad
	//if (!f) {
		//glOrtho(0.0, 1.0, 0.0, 1.0, -1.0, 1.0);
	//}
	//else {
		//gluOrtho2D(0.0, 800, 0.0, 600); //Set the 2D orthographic projection
	//}
	//f = !f;
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
	//Glut inicializa ventanas, teclado, etc.
	glutInit(&argc, argv); //Inicializa OpenGL
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB | GLUT_DEPTH); //Dibuja, nos indica que usemos RGB, y el último es un buffer de profundidad
	glutInitWindowSize(800, 600); //Tamaño de la ventana
	glutInitWindowPosition(100, 100); //Posición inicial de la ventana
	glutCreateWindow("3D Model"); //Crea la ventana, podemos poner cualquier nombre a la ventana

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

	glutDisplayFunc(display); //Se debe registrar la función de dibujado
	glutIdleFunc(idle); //For continuous updates (FPS)
	//glutIdleFunc(display); //Mientras el programa no está "haciendo algo"/inactivo se manda llamar la función para que este dibujando todo el tiempo o redibujando
	glutKeyboardFunc(handleKeypress); //Movimiento usando las teclas
	glutMotionFunc(mouseMotion); //Movimiento del mouse con botón presionado
	glutMouseFunc(mouseButton); //Detecta clics del mouse
	

	glutMainLoop(); //Esta función tiene un ciclo que se detiene hasta que se cierre la aplicación y manda llamar dentro del ciclo la función display() para estar dibujando

	return 0;   /* ISO C requires main to return int. */
}
