#pragma once
#include <iostream>
#include <vector>
#include <cmath>
constexpr double M_PI = 3.14159265358979323846;

class Matrix
{
public:
	std::vector<float> mat;

	//Constructor identity matrix
	Matrix() {
		mat = std::vector<float>(16, 0.0f);
		mat[0] = mat[5] = mat[10], mat[15] = 1.0f;
	}
	//Create translation matrix
	static Matrix translation(float tx, float ty, float tz) {
		Matrix m;
		m.mat[12] = tx;
		m.mat[13] = ty;
		m.mat[14] = tz;
		return m;
	}
	//Create scaling matrix
	static Matrix scaling(float sx, float sy, float sz) {
		Matrix m;
		m.mat[0] = sx;
		m.mat[5] = sy;
		m.mat[10] = sz;
		return m;
	}
	//Create rotation X matrix
	static Matrix rotationX(float angle) {
		Matrix m;
		float radians = angle * M_PI / 180.0f;
		m.mat[5] = cos(radians);
		m.mat[6] = -sin(radians);
		m.mat[9] = sin(radians);
		m.mat[10] = cos(radians);
		return m;
	}
	//Create rotation Y matrix
	static Matrix rotationY(float angle) {
		Matrix m;
		float radians = angle * M_PI / 180.0f;
		m.mat[0] = cos(radians);
		m.mat[2] = sin(radians);
		m.mat[8] = -sin(radians);
		m.mat[10] = cos(radians);
		return m;
	}
	//Create rotation Z matrix
	static Matrix rotationZ(float angle) {
		Matrix m;
		float radians = angle * M_PI / 180.0f;
		m.mat[0] = cos(radians);
		m.mat[1] = -sin(radians);
		m.mat[4] = sin(radians);
		m.mat[5] = cos(radians);
		return m;
	}
	//Multiply matrices
	Matrix operator*(const Matrix& other) {
		Matrix result;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				result.mat[i + j * 4] = 0.0f;
				for (int k = 0; k < 4; k++) {
					result.mat[i + j * 4] += mat[i + k * 4] * other.mat[k + j * 4];
				}
			}
		}
		return result;
	}
	//Print matrix
	void print() {
		for (int i = 0; i < 4; ++i) {
			for (int j = 0; j < 4; ++j) {
				std::cout << mat[i + j * 4] << " ";
			}
			std::cout << std::endl;
		}
	}
};

