#pragma once
#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include "Vertex.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// using namespace std; para eliminar el std::

class Matrix {
public:
	float rx, ry, rz;
	float grados_x, grados_y, grados_z;
	Vertex p[4];

	Matrix() {
		std::random_device rd; // Obtiene una semilla aleatoria
		std::mt19937 gen(rd()); // Generador de números aleatorios
		std::uniform_real_distribution<float> t_dist(-20.5f, 16.0f); // Distribución para la traslación entre 0.5 y 6
		std::uniform_real_distribution<float> r_dist(0.0f, 180.0f); // Distribución para la rotación entre 0 y 180

		p->b = p->g = p->r = 0.0f;
		for (int i = 0; i < 3; i++) {
			p[i].x = t_dist(gen);
			p[i].y = t_dist(gen);
			p[i].z = t_dist(gen);
		}

		rx = ry = rz = 0.0f;
		grados_x = r_dist(gen) * (M_PI / 180.0f);
		grados_y = r_dist(gen) * (M_PI / 180.0f);
		grados_z = r_dist(gen) * (M_PI / 180.0f);
	}

public:
	void changeP(int i, float px, float py, float pz)
	{
		if (i < 4) {
			p[i].x = px;
			p[i].y = py;
			p[i].z = pz;

		}
	}

	void changeRot(float x, float y, float z)
	{
		grados_x = x * (M_PI / 180.0f);
		grados_y = y * (M_PI / 180.0f);
		grados_z = z * (M_PI / 180.0f);
	}

	float** trasMatrix(float tx, float ty, float tz)
	{
		float t[3] = { tx, ty, tz };
		float** mat = new float* [4];

		for (int i = 0; i < 4; i++)
			mat[i] = new float[4];

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++) {
				mat[i][j] = (i == j) ? 1.0f : 0.0f;
				if (i < 3 && j == 3)
					mat[i][j] = t[i];
			}

		return mat;
	}

	float** rxMatrix(float rx)
	{
		float** mat = new float* [4];
		rx = rx * M_PI / 180.0f;

		for (int i = 0; i < 4; i++)
			mat[i] = new float[4];

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat[i][j] = (i == j) ? 1.0f : 0.0f;

		mat[1][1] = mat[2][2] = cos(rx);
		mat[1][2] = -sin(rx);
		mat[2][1] = sin(rx);

		return mat;
	}

	float** ryMatrix(float ry)
	{
		float** mat = new float* [4];
		ry = ry * M_PI / 180.0f;

		for (int i = 0; i < 4; i++)
			mat[i] = new float[4];

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat[i][j] = (i == j) ? 1.0f : 0.0f;

		mat[0][0] = mat[2][2] = cos(ry);
		mat[0][2] = sin(ry);
		mat[2][0] = -sin(ry);

		return mat;
	}

	float** rzMatrix(float rz)
	{
		float** mat = new float* [4];
		rz = rz * M_PI / 180.0f;

		for (int i = 0; i < 4; i++)
			mat[i] = new float[4];

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				mat[i][j] = (i == j) ? 1.0f : 0.0f;

		mat[0][0] = mat[1][1] = cos(rz);
		mat[0][1] = -sin(rz);
		mat[1][0] = sin(rz);

		return mat;
	}

	float** multMatrix(float** A, float** B)
	{
		float** C = new float* [4];

		for (int i = 0; i < 4; i++)
			C[i] = new float[4];

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				C[i][j] = 0;

		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 4; j++)
				for (int k = 0; k < 4; k++)
					C[i][j] += A[i][k] * B[k][j];

		return C;
	}

	float** rotMatrix(Vertex c)
	{
		float** res = multMatrix(trasMatrix(c.x, c.y, c.z), rzMatrix(rz));
		res = multMatrix(res, ryMatrix(ry));
		res = multMatrix(res, rxMatrix(rx));
		res = multMatrix(res, trasMatrix(c.x * -1, c.y * -1, c.z * -1));
		return res;
	}

	Vertex resVector(float** A, Vertex B)
	{
		Vertex v;

		v.b = B.b;
		v.g = B.g;
		v.r = B.r;

		v.x = A[0][0] * B.x + A[0][1] * B.y + A[0][2] * B.z + A[0][3];
		v.y = A[1][0] * B.x + A[1][1] * B.y + A[1][2] * B.z + A[1][3];
		v.z = A[2][0] * B.x + A[2][1] * B.y + A[2][2] * B.z + A[2][3];

		return v;
	}
};